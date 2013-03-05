#!/usr/bin/python

import sys
import math
from lxml import objectify
from lxml import etree

#This program converts a SUMO traffic network to Sim Mobility format (this includes 
# flipping the network for driving on the left).
#Note: Run SUMO like so:
#   ~/sumo/bin/netgenerate --rand -o sumo.net.xml --rand.iterations=200 --random -L 2

#Note that this program runs about 10x faster on Python3 for some reason


#Our container class
class RoadNetwork:
  def __init__(self):
    self.nodes = {}    #origId => Node
    self.links = {}    #origId => Link
    self.lanes = {}    #origId => Lane
    self.turnings = [] #LaneConnector


#Simple classes. IDs are always strings
class Node:
  def __init__(self, nodeId, xPos, yPos):
    if not (nodeId and xPos and yPos):
      raise Exception('Null parameters in Node constructor')
    self.nodeId = str(nodeId)
    self.guid = None
    self.pos = Point(float(xPos), float(yPos))
    self.is_uni = None

  def isUni(self):
    if self.is_uni is None:
      raise Exception('isUni not defined for Node')
    return self.is_uni

class Link:
  def __init__(self, linkId, fromNode, toNode):
    if not (linkId and fromNode and toNode):
      raise Exception('Null parameters in Link constructor')
    self.linkId = str(linkId)
    self.guid = None
    self.fromNode = str(fromNode)
    self.toNode = str(toNode)
    self.segments = [] #List of segment IDs

class Edge:
  def __init__(self, edgeId, fromNode, toNode):
    if not (edgeId and fromNode and toNode):
      raise Exception('Null parameters in Edge constructor')
    self.edgeId = str(edgeId)
    self.guid = None
    self.fromNode = str(fromNode)
    self.toNode = str(toNode)
    self.lanes = []
    self.lane_edges = []

#Note that "from/toLaneId" are zero-numbered, not actual lane IDs
class LaneConnector:
  def __init__(self, fromSegment, toSegment, fromLaneId, toLaneId, laneFromOrigId, laneToOrigId):
    self.fromSegment = fromSegment  #These are references
    self.toSegment = toSegment      #These are references
    self.fromLaneId = fromLaneId
    self.toLaneId = toLaneId
    self.laneFromOrigId = laneFromOrigId
    self.laneToOrigId = laneToOrigId

class Lane:
  def __init__(self, laneId, shape):
    if not (laneId and shape):
      raise Exception('Null parameters in Lane constructor')
    self.laneId = str(laneId)
    self.guid = None
    self.shape = Shape(shape)

class LaneEdge:
  def __init__(self, points):
    self.points = points  #Just an array of Points

#A basic vector (in the geometrical sense)
class DynVect:
  def __init__(self, start, end):
    #Ensure that we copy.
    self.pos = Point(float(start.x), float(start.y))
    self.mag = Point(float(end.x)-start.x, float(end.y)-start.y)

  def getPos(self):
    return self.pos

  def translate(self):
    self.pos.x += self.mag.x
    self.pos.y += self.mag.y
    return self

  def scaleVectTo(self, amount):
    #Factoring in the unit vector by dividing early is more accurate.
    factor = amount/self.getMagnitude()
    self.mag.x = factor * self.mag.x
    self.mag.y = factor * self.mag.y
    return self

  def getMagnitude(self):
    return math.sqrt(self.mag.x**2.0 + self.mag.y**2.0)

  def flipNormal(self, clockwise):
    sign =  1.0 if clockwise else -1.0
    newX = self.mag.y*sign
    newY = -self.mag.x*sign
    self.mag.x = newX
    self.mag.y = newY
    return self

  def rotateRight(self): #Flip this vector 90 degrees clockwise around the origin.
    return self.flipNormal(True)

  def rotateLeft(self):  #Flip this vector 90 degrees counter-clockwise around the origin.
    return self.flipNormal(False)


#NOTE on Shapes, from the SUMO user's guide
#The start and end node are omitted from the shape definition; an example: 
#    <edge id="e1" from="0" to="1" shape="0,0 0,100"/> 
#    describes an edge that after starting at node 0, first visits position 0,0 
#    than goes one hundred meters to the right before finally reaching the position of node 1
class Shape:
  def __init__(self, pts):
    pts = pts.split(' ')

    self.points = []
    for pair in pts:
      pair = pair.split(',')
      self.points.append(Point(float(pair[0]), float(pair[1])))


class Point:
  def __init__(self, x, y):
    self.x = x
    self.y = y

  def __repr__(self):
    return "Point(%f,%f)" % (self.x, self.y)

  def __str__(self):
    return "(%f,%f)" % (self.x, self.y)



def parse_edge_sumo(e, links, lanes):
    #Sanity check
    if e.get('function')=='internal':
      raise Exception('Node with from/to should not be internal')

    #Add a new edge/link (both with the same "sumo id")
    res = Link(e.get('id'), e.get('from'), e.get('to'))
    primEdge = Edge(res.linkId, res.fromNode, res.toNode) #Each sumo link has one "primary edge"
    res.segments.append(primEdge)
    links[res.linkId] = res

    #Add child Lanes
    laneTags = e.xpath("lane")
    for l in laneTags:
      newLane = Lane(l.get('id'), l.get('shape'))
      primEdge.lanes.append(newLane)
      lanes[newLane.laneId] = newLane


def parse_link_osm(lk, nodes, links, lanes, globalIdCounter):
    #Skip footways.
    hwTag = lk.xpath("tag[@k='highway']")
    if len(hwTag) > 0:
      if (hwTag[0].get('v')=='footway'):
        return globalIdCounter

    #First, build up a series of Node IDs
    nodeIds = []
    for ndItem in lk.iter("nd"):
      nId = ndItem.get('ref')
      if not nId:
        raise Exception('No "ref" for "nd" in "way"')
      nodeIds.append(nId)

    #Ensure we have at least two points.
    if len(nodeIds) < 2:
      print('Warning: Skipping an OSM Way, as it does not have at least 2 node refs: %s' % lk.get('id'))
      return globalIdCounter

    #Attempt to extract the lane count; default to 1
    numLanes = 1
    numLanesTag = lk.xpath("tag[@k='lanes']")
    if len(numLanesTag) > 0:
      numLanes = int(numLanesTag[0].get('v'))

    #First, add a new Link
    res = Link(lk.get('id'), nodeIds[0], nodeIds[-1])
    links[res.linkId] = res

    #Now add one segment per node pair. (For now Sim Mobility does not handle polylines that well).
    for i in range(len(nodeIds)-1):
      #Retrieve the node IDs
      fromNode = nodeIds[i]
      toNode = nodeIds[i+1]

      #Make an edge
      e = Edge(globalIdCounter, fromNode, toNode)
      globalIdCounter += 1
      res.segments.append(e)

      #Sanity check
      if not (fromNode in nodes and toNode in nodes):
        raise Exception('Way references unknown Nodes')

      #We create two vectors, one at the fromNode and one at the toNode. 
      LaneWidth = 3.4
      startVec = DynVect(nodes[fromNode].pos, nodes[toNode].pos)
      endVec = DynVect(nodes[toNode].pos, nodes[fromNode].pos)

      #We can provide a simple "intersection" buffer if the total lane length allows it.
      BufferSize = 10.0
      if (startVec.getMagnitude() > 1.1*BufferSize):
         startVec.scaleVectTo(BufferSize/2.0).translate()
         endVec.scaleVectTo(BufferSize/2.0).translate()

      #Point both away from the median line.
      #These are incremented by a faux-lane-width in order to generate basic lane sizes.
      startVec.scaleVectTo(LaneWidth/2.0).rotateLeft().translate().scaleVectTo(LaneWidth)
      endVec.scaleVectTo(LaneWidth/2.0).rotateRight().translate().scaleVectTo(LaneWidth)

      #Add child Lanes for each Segment
      for lIt in range(numLanes):
        #Fake the shape; we'll have to translate it right back unfortunately.
        shapeStr = '%f,%f %f,%f' % (startVec.getPos().x, startVec.getPos().y, endVec.getPos().x, endVec.getPos().y)
        startVec.translate()
        endVec.translate()

        #Make the new lane.
        #Lane IDs (here) probably don't matter, since they are only used internally.
        newLane = Lane(len(lanes)+100, shapeStr)
        e.lanes.append(newLane)
        lanes[newLane.laneId] = newLane

      #SUMO expects lanes in reverse; we'll do the same here (for now).
      e.lanes.reverse()

    return globalIdCounter



def project_coords(wgsRev, utmZone, lat, lon):
  from LatLongUTMconversion import LLtoUTM

  #Make sure we have both params, convert to float.
  if not (lat and lon):
    raise Exception('lat/lon required in project_coords')
  lat = float(lat)
  lon = float(lon)

  #Make sure they are using the latest standard
  if (wgsRev.replace(' ', '') != 'WGS84'):
    raise Exception('Deprecated WGS specification (only WGS 84 supported)')

  #Now, perform the projection. Make sure our result matches our expectations.
  (resZone, x, y) = LLtoUTM(23, lat, lon)
  if (utmZone.replace(' ', '') != "UTM"+resZone.replace(' ', '')):
    raise Exception('Resultant UTM zone (%s) does not match expected zone (%s).' % (utmZone, resZone))

  #All is good; return a Point
  return Point(x,y)


def parse_junctions_sumo(j, nodes):
    #Add a new Node
    res = Node(j.get('id'), j.get('x'), j.get('y'))
    nodes[res.nodeId] = res

def parse_nodes_osm(n, nodes):
    #Nodes are slightly complicated by the fact that they use lat/long.
    projected = project_coords('WGS 84', 'UTM 48N', n.get('lat'), n.get('lon'))  #Returns a point

    #Add a new Node
    res = Node(n.get('id'), projected.x, projected.y)
    nodes[res.nodeId] = res


def remove_unused_nodes(nodes, links):
  #This function counts the number of Segments which reference a Node. If it's zero, that 
  # Node is pruned. Else, it's marked as a uni/multi Node based on count.
  segmentsAt = {} #node => [edge,edge,]
  for lk in links.values():
    #Save time later:
    nodes[lk.segments[0].fromNode].is_uni = False
    nodes[lk.segments[-1].toNode].is_uni = False

    for e in lk.segments:
      #Add it if it doesn't exist.
      fromN = nodes[e.fromNode]
      toN = nodes[e.toNode]
      if not fromN in segmentsAt:
        segmentsAt[fromN] = []
      if not toN in segmentsAt:
        segmentsAt[toN] = []

      #Increment
      segmentsAt[fromN].append(e)
      segmentsAt[toN].append(e)

  #Now assign the uni/multi Node property:
  for n in segmentsAt.keys():
    segs = segmentsAt[n]
    if len(segs)==0:
      raise Exception('Empty segment accidentally added.')

    #Don't set it unless it's still unset.
    if n.is_uni is None:
      #This should fail:
      if len(segs)==1:
        raise Exception('Node should be a MultiNode...')
        #n.is_uni = True

      #Simple. 
      #TODO: What if len(segs)==4? We can share Nodes, right?
      elif len(segs)>2:
        n.is_uni = False

      #Somewhat harder:
      else:
        #The segments here should share one node (this node) and then go to different destinations.
        n1 = segs[0].fromNode if segs[0].toNode==n.nodeId else segs[0].toNode
        n2 = segs[1].fromNode if segs[1].toNode==n.nodeId else segs[1].toNode
        n.is_uni = (n1 != n2)


  #Finally, do a quick test for our OSM data:
  for lk in links.values():
    seg_nodes = [nodes[lk.segments[0].fromNode]]
    for e in lk.segments:
      seg_nodes.append(nodes[e.toNode])

    #Begins and ends at a MultiNode? (This shouldn't fail)
    if seg_nodes[0].isUni():
      raise Exception('UniNode found (%s) at start of Segment where a MultiNode was expected.' % seg_nodes[0].nodeId)
    if seg_nodes[-1].isUni():
      raise Exception('UniNode found (%s) at end of Segment where a MultiNode was expected.' % seg_nodes[-1].nodeId)

    #Ensure middle nodes are UniNodes. Note that this is not strictly required; we'll just
    #   have to false-segment the nodes that trigger this. 
    #NOTE: This currently affects 10% of all Nodes. 
    for i in range(1, len(seg_nodes)-2):
      if not seg_nodes[i].isUni():
        raise Exception('Non-uni node (%s) in middle of a Segment.' % seg_nodes[i].nodeId)

  #We can cheat a little here: Nodes with no references won't even be in our result set.
  nodes.clear()
  for n in segmentsAt.keys():
    nodes[n.nodeId] = n


class InOut:
  def __init__(self):
    self.incoming = []
    self.outgoing = []

def make_lane_connectors(rn):
  #First, make a list of all "incoming" and "outgoing" edges at a given node
  lookup = {}  #nodeId => InOut
  for lk in rn.links.values():
    for e in lk.segments:
      #Give it an entry
      if not (e.fromNode in lookup):
        lookup[e.fromNode] = InOut()
      if not (e.toNode in lookup):
        lookup[e.toNode] = InOut()

    #Append
    lookup[e.toNode].incoming.append(e)
    lookup[e.fromNode].outgoing.append(e)

  #Now make a set of lane connectors from all "incoming" to all "outgoing" (except U-turns) at a Node
  for n in rn.nodes.values():
    for fromEdge in lookup[n.nodeId].incoming:
      for toEdge in lookup[n.nodeId].outgoing:
        if (fromEdge.fromNode==toEdge.toNode and fromEdge.toNode==toEdge.fromNode):
          continue

        #The looping gets even deeper!
        for fromLaneID in range(len(fromEdge.lanes)):
          for toLaneID in range(len(toEdge.lanes)):
            rn.turnings.append(LaneConnector(fromEdge, toEdge, fromLaneID, toLaneID, fromEdge.lanes[fromLaneID].laneId, toEdge.lanes[toLaneID].laneId))


def check_and_flip_and_scale(rn):
  #Save the maximum X co-ordinate, minimum Y
  maxX = None

  #Currently SimMobility can't take negative y coords
  minX = 0
  minY = 0

  #Iteraet through Edges; check node IDs
  for lk in rn.links.values():
    for e in lk.segments:
      if not ((e.fromNode in rn.nodes) and (e.toNode in rn.nodes)):
        raise Exception('Edge references unknown Node ID')

  #Iterate through Nodes, Lanes (Shapes) and check all points here too.
  for n in rn.nodes.values():
    maxX = max((maxX if maxX else n.pos.x),n.pos.x)
    minX = min(minX, n.pos.x)
    minY = min(minY, n.pos.y)
  for l in rn.lanes.values():
    for p in l.shape.points:
      maxX = max((maxX if maxX else p.x),p.x)
      minX = min(minX, p.x)
      minY = min(minY, p.y)

  #Add some buffer space to the y-value (for lane edge lines)
  minX -= 10
  minY -= 10

  #Now invert all x co-ordinates, and scale by 100 (to cm)
  for n in rn.nodes.values():
    n.pos.x = (-minX + maxX - n.pos.x) * 100
    n.pos.y = (-minY + n.pos.y) * 100
  for l in rn.lanes.values():
    for p in l.shape.points:
      p.x = (-minX + maxX - p.x) * 100
      p.y = (-minY + p.y) * 100


def mostly_parallel(first, second):
  #Cutoff point for considering lines *not* parallel
  #You can increase this as your lines skew, but don't go too high.
  #Cutoff = 3.0
  Cutoff = 50.0  #TODO: Temp, while we work on OSM data.

  #Both/one vertical?
  fDx= first[-1].x-first[0].x
  sDx = second[-1].x-second[0].x
  if (fDx==0 or sDx==0):
    return (fDx-sDx) < Cutoff

  #Calculate slope
  mFirst = float(first[-1].y-first[0].y) / float(fDx)
  mSecond = (second[-1].y-second[0].y) / float(sDx)
  return abs(mFirst-mSecond) < Cutoff



#Assumes first/second are parallel lines
def get_line_dist(first, second):
  #Slope is vertical?
  dx= first[-1].x-first[0].x
  if (dx==0):
    return abs(first[0].y-second[0].y)

  #Otherwise, get y-intercept for each line.
  m1 = (first[-1].y-first[0].y) / float(dx)
  m2 = (second[-1].y-second[0].y) / float(dx)
  m = (m1+m2)/2.0  #We use the average slope just in case the lines aren't *quite* parallel
  fB = first[0].y - m * first[0].x
  sB = second[0].y - m * second[0].x

  #And then we have a magic formula:
  return abs(sB-fB) / math.sqrt(m**2 + 1)



def make_lane_edges(rn):
  for lk in rn.links.values():
    for e in lk.segments:
      #All lanes are relative to our Segment line
      segLine = [rn.nodes[e.fromNode].pos, rn.nodes[e.toNode].pos]

      #We need the lane widths. To do this geometrically, first take lane line one (-1) and compare the slopes:
      zeroLine = [e.lanes[-1].shape.points[0],e.lanes[-1].shape.points[-1]]
      if not mostly_parallel(segLine, zeroLine):
        raise Exception("Can't convert edge %s; lines are not parallel: [%s=>%s] and [%s=>%s]" % (e.edgeId, segLine[0], segLine[-1], zeroLine[0], zeroLine[-1]))

      #Now that we know the lines are parallel, get the distance between them. This should be half the lane width
      halfW = get_line_dist(segLine, zeroLine)

      #Add lane line 1 (actually -1, since sumo lists are in reverse order) shifted RIGHT to give us line zero
      zeroStart = DynVect(zeroLine[0], zeroLine[1])
      zeroStart.rotateRight().scaleVectTo(halfW).translate()
      zeroEnd = DynVect(zeroLine[1], zeroLine[0])
      zeroEnd.rotateLeft().scaleVectTo(halfW).translate()
      e.lane_edges.append(LaneEdge([zeroStart.getPos(), zeroEnd.getPos()]))

      #Now add each remaining lane (including 1, again) shifted LEFT to give us their expected location.
      for i in reversed(range(len(e.lanes))):
        currLine = [e.lanes[i].shape.points[0],e.lanes[i].shape.points[-1]]
        currStart = DynVect(currLine[0], currLine[1])
        currStart.rotateLeft().scaleVectTo(halfW).translate()
        currEnd = DynVect(currLine[1], currLine[0])
        currEnd.rotateRight().scaleVectTo(halfW).translate()
        e.lane_edges.append(LaneEdge([currStart.getPos(), currEnd.getPos()]))



def print_old_format(rn):
  #Open, start writing
  f = open('out.txt', 'w')
  f.write('("simulation", 0, 0, {"frame-time-ms":"100",})\n')

  #Nodes
  for n in rn.nodes.values():
    f.write('("multi-node", 0, %d, {"xPos":"%d","yPos":"%d"})\n' % (n.guid, n.pos.x, n.pos.y))

  #Links, edges
  for lk in rn.links.values():
    #Write the link (and its forward segments)
    fromId = rn.nodes[lk.fromNode].guid
    toId = rn.nodes[lk.toNode].guid
    f.write('("link", 0, %d, {"road-name":"","start-node":"%d","end-node":"%d","fwd-path":"[' % (lk.guid, fromId, toId))
    for e in lk.segments:
      f.write('%d,' % e.guid)
    f.write(']",})\n')

    #Write the segments one-by-one
    for e in lk.segments:
      fromId = rn.nodes[e.fromNode].guid
      toId = rn.nodes[e.toNode].guid
      f.write('("road-segment", 0, %d, {"parent-link":"%d","max-speed":"65","width":"%d","lanes":"%d","from-node":"%d","to-node":"%d"})\n' % (e.guid, lk.guid,(250*len(e.lanes)), len(e.lanes), fromId, toId))

      #Lanes are somewhat more messy
      #Note that "lane_id" here just refers to lane line 0, since lanes are grouped by edges in this output format. (Confusingly)
      f.write('("lane", 0, %d, {"parent-segment":"%d",' % (e.lanes[0].guid, e.guid))

      #Each lane component
      i = 0
      for l in e.lane_edges:
        f.write('"lane-%d":"[' % (i))

        #Lane lines are mildly more complicated, since we need lane *edge* lines.
        for p in l.points:
          f.write('(%d,%d),' % (p.x, p.y))
        f.write(']",')
        i+=1

      #And finally
      f.write('})\n')

  #Write all Lane Connectors
  currLC_id = 1
  for lc in rn.turnings:
    f.write('("lane-connector", 0, %d, {"from-segment":"%d","from-lane":"%d","to-segment":"%d","to-lane":"%d",})\n' % (currLC_id, lc.fromSegment.guid, lc.fromLaneId, lc.toSegment.guid, lc.toLaneId))
    currLC_id += 1

  #Done
  f.close()


def write_xml_multinodes(f, nodes, links, lanes, turnings):
  #Build a lookup of "edges at nodes"
  lookup = {} #nodeID => [edges(refs)]
  for lk in links.values():
    for e in lk.segments:
      if not (e.fromNode in lookup):
        lookup[e.fromNode] = []
      if not (e.toNode in lookup):
        lookup[e.toNode] = []
      lookup[e.fromNode].append(e)
      lookup[e.toNode].append(e)

  #Now write it.
  for n in nodes.values():
    f.write('          <Intersection>\n')
    f.write('            <nodeID>%d</nodeID>\n' % n.guid)
    f.write('            <location>\n')
    f.write('              <xPos>%d</xPos>\n' % n.pos.x)
    f.write('              <yPos>%d</yPos>\n' % n.pos.y)
    f.write('            </location>\n')

    f.write('            <roadSegmentsAt>\n')
    if (n.nodeId in lookup):
      for e in lookup[n.nodeId]:
        f.write('              <segmentID>%d</segmentID>\n' % e.guid)
    f.write('            </roadSegmentsAt>\n')

    #Wee need another lookup; this one for lane connectors
    look_lc = {} #fromSegment => [LaneConnector]
    if (n.nodeId in lookup):
      for lc in turnings:
        if (lc.fromSegment in lookup[n.nodeId]) and (lc.toSegment in lookup[n.nodeId]):
          if not (lc.fromSegment in look_lc):
            look_lc[lc.fromSegment] = []
          look_lc[lc.fromSegment].append(lc)

    #Write connectors
    f.write('            <Connectors>\n')
    for rs in look_lc.keys():
      f.write('              <MultiConnectors>\n')
      f.write('                <RoadSegment>%d</RoadSegment>\n' % rs.guid)
      f.write('                <Connectors>\n')
      for lc in look_lc[rs]:
        f.write('                  <Connector>\n')
        f.write('                    <laneFrom>%d</laneFrom>\n' % lanes[lc.laneFromOrigId].guid)
        f.write('                    <laneTo>%d</laneTo>\n' % lanes[lc.laneToOrigId].guid)
        f.write('                  </Connector>\n')
      f.write('                </Connectors>\n')
      f.write('              </MultiConnectors>\n')
    f.write('            </Connectors>\n')

    f.write('          </Intersection>\n')


def write_xml_nodes(f, rn):
  #Write Nodes
  f.write('      <Nodes>\n')
  f.write('        <UniNodes/>\n')
  f.write('        <Intersections>\n')
  write_xml_multinodes(f, rn.nodes, rn.links, rn.lanes, rn.turnings)
  f.write('        </Intersections>\n')
  f.write('      </Nodes>\n')


#Distance between 2 nodes/points
def dist(m, n):
  #Convert to Point
  if isinstance(m, Node):
    m = m.pos
  if isinstance(n, Node):
    n = n.pos

  #Calc distance
  dx = n.x - m.x;
  dy = n.y - m.y;
  return math.sqrt(dx**2 + dy**2);



def write_xml_lane_edge_polylines(f, lane_edges):
  #Relatively simple layout
  curr_id = 0
  for le in lane_edges:
    f.write('                <laneEdgePolyline_cached>\n')
    f.write('                  <laneNumber>%d</laneNumber>\n' % curr_id)
    curr_id += 1
    f.write('                  <polyline>\n')
    pt_id = 0
    for p in le.points:
      f.write('                    <PolyPoint>\n')
      f.write('                      <pointID>%d</pointID>\n' % pt_id)
      pt_id += 1
      f.write('                      <location>\n')
      f.write('                        <xPos>%d</xPos>\n' % p.x)
      f.write('                        <yPos>%d</yPos>\n' % p.y)
      f.write('                      </location>\n')
      f.write('                    </PolyPoint>\n')
    f.write('                  </polyline>\n')
    f.write('                </laneEdgePolyline_cached>\n')


def write_xml_lanes(f, lanes, est_width):
  #Lanes just have a lot of properties; we fake nearly all of them.
  for l in lanes:
    f.write('                <Lane>\n')
    f.write('                  <laneID>%d</laneID>\n' % l.guid)
    f.write('                  <width>%d</width>\n' % est_width)
    f.write('                  <can_go_straight>true</can_go_straight>\n')
    f.write('                  <can_turn_left>true</can_turn_left>\n')
    f.write('                  <can_turn_right>true</can_turn_right>\n')
    f.write('                  <can_turn_on_red_signal>false</can_turn_on_red_signal>\n')
    f.write('                  <can_change_lane_left>true</can_change_lane_left>\n')
    f.write('                  <can_change_lane_right>true</can_change_lane_right>\n')
    f.write('                  <is_road_shoulder>false</is_road_shoulder>\n')
    f.write('                  <is_bicycle_lane>false</is_bicycle_lane>\n')
    f.write('                  <is_pedestrian_lane>false</is_pedestrian_lane>\n')
    f.write('                  <is_vehicle_lane>true</is_vehicle_lane>\n')
    f.write('                  <is_standard_bus_lane>false</is_standard_bus_lane>\n')
    f.write('                  <is_whole_day_bus_lane>false</is_whole_day_bus_lane>\n')
    f.write('                  <is_high_occupancy_vehicle_lane>false</is_high_occupancy_vehicle_lane>\n')
    f.write('                  <can_freely_park_here>false</can_freely_park_here>\n')
    f.write('                  <can_stop_here>false</can_stop_here>\n')
    f.write('                  <is_u_turn_allowed>false</is_u_turn_allowed>\n')
    f.write('                  <PolyLine>\n')
    pt_id = 0
    for p in l.shape.points:
      f.write('                    <PolyPoint>\n')
      f.write('                      <pointID>%d</pointID>\n' % pt_id)
      pt_id += 1
      f.write('                      <location>\n')
      f.write('                        <xPos>%d</xPos>\n' % p.x)
      f.write('                        <yPos>%d</yPos>\n' % p.y)
      f.write('                      </location>\n')
      f.write('                    </PolyPoint>\n')
    f.write('                  </PolyLine>\n')
    f.write('                </Lane>\n')


def write_xml_segment(f, e, nodes, links, turnings):
  #Each Link has only 1 segment
  seg_width = dist(e.lane_edges[0].points[0],e.lane_edges[-1].points[0])
  f.write('            <Segment>\n')
  f.write('              <segmentID>%d</segmentID>\n' % e.guid)
  f.write('              <startingNode>%d</startingNode>\n' % nodes[e.fromNode].guid)
  f.write('              <endingNode>%d</endingNode>\n' % nodes[e.toNode].guid)
  f.write('              <maxSpeed>60</maxSpeed>\n')
  f.write('              <Length>%d</Length>\n' % dist(nodes[e.fromNode],nodes[e.toNode]))
  f.write('              <Width>%d</Width>\n' % seg_width)
  f.write('              <polyline>\n')
  f.write('                <PolyPoint>\n')
  f.write('                  <pointID>0</pointID>\n')
  f.write('                  <location>\n')
  f.write('                    <xPos>%d</xPos>\n' % nodes[e.fromNode].pos.x)
  f.write('                    <yPos>%d</yPos>\n' % nodes[e.fromNode].pos.y)
  f.write('                  </location>\n')
  f.write('                </PolyPoint>\n')
  f.write('                <PolyPoint>\n')
  f.write('                  <pointID>1</pointID>\n')
  f.write('                  <location>\n')
  f.write('                    <xPos>%d</xPos>\n' % nodes[e.toNode].pos.x)
  f.write('                    <yPos>%d</yPos>\n' % nodes[e.toNode].pos.y)
  f.write('                  </location>\n')
  f.write('                </PolyPoint>\n')
  f.write('              </polyline>\n')
  f.write('              <laneEdgePolylines_cached>\n')
  write_xml_lane_edge_polylines(f, e.lane_edges)
  f.write('              </laneEdgePolylines_cached>\n')
  f.write('              <Lanes>\n')
  write_xml_lanes(f, e.lanes, seg_width/float(len(e.lanes)))
  f.write('              </Lanes>\n')
  f.write('              <Obstacles>\n')  #No obstacles for now, but we might generate pedestrian crossings later.
  f.write('              </Obstacles>\n')
  f.write('            </Segment>\n')

def write_xml_links(f, rn):
  #Write Links
  f.write('      <Links>\n')
  for lk in rn.links.values():
    f.write('        <Link>\n')
    f.write('          <linkID>%d</linkID>\n' % lk.guid)
    f.write('          <roadName/>\n')
    f.write('          <StartingNode>%d</StartingNode>\n' % rn.nodes[lk.fromNode].guid)
    f.write('          <EndingNode>%d</EndingNode>\n' % rn.nodes[lk.toNode].guid)
    f.write('          <Segments>\n')
    for e in lk.segments:
      write_xml_segment(f, e, rn.nodes, rn.links, rn.turnings)
    f.write('          </Segments>\n')
    f.write('        </Link>\n')
  f.write('      </Links>\n')


def print_xml_format(rn):
  #Open, start writing
  f = open('simmob.network.xml', 'w')
  f.write('<?xml version="1.0" encoding="utf-8" ?>\n')
  f.write('<geo:SimMobility\n')
  f.write('    xmlns:geo="http://www.smart.mit.edu/geo"\n')
  f.write('    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" \n')
  f.write('    xsi:schemaLocation="http://www.smart.mit.edu/geo   ../../Basic/shared/geospatial/xmlLoader/geo10.xsd">\n\n')

  f.write('    <GeoSpatial>\n')
  f.write('    <RoadNetwork>\n')
  write_xml_nodes(f, rn)
  write_xml_links(f, rn)
  f.write('    </RoadNetwork>\n')
  f.write('    </GeoSpatial>\n')
  f.write('</geo:SimMobility>\n')

  #Done
  f.close()


#Sim Mobility doesn't strictly require globally unique IDs, but they make debugging easier
#  (and may be related to a glitch in the StreetDirectory).
def assign_unique_ids(rn, currId):
  for n in rn.nodes.values():
    n.guid = currId
    currId += 1
  for lk in rn.links.values():
    lk.guid = currId
    currId += 1
    for e in lk.segments:
      e.guid = currId
      currId += 1
  for l in rn.lanes.values():
    l.guid = currId
    currId += 1




def parse_all_sumo(rootNode, rn):
  #For each junction
  junctTags = rootNode.xpath('/net/junction')
  for j in junctTags:
    parse_junctions_sumo(j, rn.nodes)

  #For each edge; ignore "internal"
  edgeTags = rootNode.xpath("/net/edge[(@from)and(@to)]")
  for e in edgeTags:
    parse_edge_sumo(e, rn.links, rn.lanes)



def parse_all_osm(rootNode, rn):
  #All nodes
  nodeTags = rootNode.xpath('/osm/node')
  for n in nodeTags:
    parse_nodes_osm(n, rn.nodes)

  #All links
  linksTags = rootNode.xpath("/osm/way")
  globalIdCounter = 1000 #For edges
  for lk in linksTags:
    globalIdCounter = parse_link_osm(lk, rn.nodes, rn.links, rn.lanes, globalIdCounter)



def run_main(inFileName):
  #Resultant datastructure
  rn = RoadNetwork()

  #Load, parse
  inFile = open(inFileName)
  doc = objectify.parse(inFile)

  #What kind of file is this?
  format = 'U'  #U[nknown], [S]umo, [O]penStreetMap
  if len(doc.xpath("/net"))>0:
    format = 'S'
  elif len(doc.xpath("/osm"))>0:
    format = 'O'

  #Parse edges, lanes, nodes
  if format=='S':
    parse_all_sumo(doc, rn)
  elif format=='O':
    parse_all_osm(doc, rn)
  else:
    raise Exception('Unknown road network format: ' + inFileName)


  #Remove junction nodes which aren't referenced by anything else.
  nodesPruned = len(rn.nodes)
  remove_unused_nodes(rn.nodes, rn.links)
  nodesPruned -= len(rn.nodes)
  print("Pruned unreferenced nodes: %d" % nodesPruned)

  #Give these unique output IDs
  assign_unique_ids(rn, 1000)

  #Check network properties; flip X coordinates
  check_and_flip_and_scale(rn)

  #Create N+1 lane edges from N lanes
  make_lane_edges(rn)

  #Create lane connectors from/to every lane *except* going backwards. This is an 
  # oversimplification, but it applies to all generated SUMO networks.
  make_lane_connectors(rn)

  #Before printing the XML network, we should print an "out.txt" file for 
  #  easier visual verification with our old GUI.
  print_old_format(rn)

  #Now print the network in XML format, for use with the actual software.
  print_xml_format(rn)


if __name__ == "__main__":
  if len(sys.argv) < 2:
    print ('Usage:\n' , sys.argv[0] , '<in_file.net.xml>')
    sys.exit(0)

  run_main(sys.argv[1])
  print("Done")



