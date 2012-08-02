// Not copyrighted - public domain.
//
// This sample parser implementation was generated by CodeSynthesis XSD,
// an XML Schema to C++ data binding compiler. You may use it in your
// programs without any restrictions.
//

#ifndef GEO8_PIMPL_HPP
#define GEO8_PIMPL_HPP

#include "geo8-pskel.hpp"
#include "conf/simpleconf.hpp"

namespace geo
{
  class SegmentType_t_pimpl: public virtual SegmentType_t_pskel,
    public ::xml_schema::string_pimpl
  {
    public:
    virtual void
    pre ();

    virtual std::string
    post_SegmentType_t ();
  };

  class ObstacleType_t_pimpl: public virtual ObstacleType_t_pskel,
    public ::xml_schema::string_pimpl
  {
    public:
    virtual void
    pre ();

    virtual void
    post_ObstacleType_t ();
  };

  class LaneType_t_pimpl: public virtual LaneType_t_pskel,
    public ::xml_schema::string_pimpl
  {
    public:
    virtual void
    pre ();

    virtual void
    post_LaneType_t ();
  };

  class obstacle_t_pimpl: public virtual obstacle_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    obstacleID (const ::std::string&);

    virtual void
    obstacleType ();

    virtual sim_mob::RoadItem*
    post_obstacle_t ();
  };

  class obstacles_t_pimpl: public virtual obstacles_t_pskel
  {
	  std::map<centimeter_t, const RoadItem*> roadItems;
    public:
    virtual void
    pre ();

    virtual void
    obstacle (sim_mob::RoadItem*);

    virtual std::map<centimeter_t,const RoadItem*>
    post_obstacles_t ();
  };

  class Point2D_t_pimpl: public virtual Point2D_t_pskel
  {
	  sim_mob::Point2D point2D;
    public:
    virtual void
    pre ();

    virtual void
    xPos (unsigned int);

    virtual void
    yPos (unsigned int);

    virtual sim_mob::Point2D
    post_Point2D_t ();
  };

  class PolyPoint_t_pimpl: public virtual PolyPoint_t_pskel
  {
	  sim_mob::Point2D point2D;
    public:
    virtual void
    pre ();

    virtual void
    pointID (const ::std::string&);

    virtual void
    location (sim_mob::Point2D);

    virtual sim_mob::Point2D
    post_PolyPoint_t ();
  };

  class PolyLine_t_pimpl: public virtual PolyLine_t_pskel
  {
	  std::vector<sim_mob::Point2D> polyLine;
    public:
    virtual void
    pre ();

    virtual void
    PolyPoint (sim_mob::Point2D);

    virtual std::vector<sim_mob::Point2D>
    post_PolyLine_t ();
  };

  class lane_t_pimpl: public virtual lane_t_pskel
  {
	  sim_mob::Lane *lane;
    public:
    virtual void
    pre ();

    virtual void
    laneID (const ::std::string&);

    virtual void
    width (unsigned int);

    virtual void
    can_go_straight (bool);

    virtual void
    can_turn_left (bool);

    virtual void
    can_turn_right (bool);

    virtual void
    can_turn_on_red_signal (bool);

    virtual void
    can_change_lane_left (bool);

    virtual void
    can_change_lane_right (bool);

    virtual void
    is_road_shoulder (bool);

    virtual void
    is_bicycle_lane (bool);

    virtual void
    is_pedestrian_lane (bool);

    virtual void
    is_vehicle_lane (bool);

    virtual void
    is_standard_bus_lane (bool);

    virtual void
    is_whole_day_bus_lane (bool);

    virtual void
    is_high_occupancy_vehicle_lane (bool);

    virtual void
    can_freely_park_here (bool);

    virtual void
    can_stop_here (bool);

    virtual void
    is_u_turn_allowed (bool);

    virtual void
    PolyLine (std::vector<sim_mob::Point2D>);

    virtual sim_mob::Lane*
    post_lane_t ();
  };

  class connector_t_pimpl: public virtual connector_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    laneFrom (const ::std::string&);

    virtual void
    laneTo (const ::std::string&);

    virtual sim_mob::LaneConnector*
    post_connector_t ();
  };

  class connectors_t_pimpl: public virtual connectors_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    Connector (sim_mob::LaneConnector*);

    virtual void
    post_connectors_t ();
  };

  class fwdBckSegments_t_pimpl: public virtual fwdBckSegments_t_pskel
  {
	  std::vector<sim_mob::RoadSegment*> Segments;
    public:
    virtual void
    pre ();

    virtual void
    Segment (sim_mob::RoadSegment*);

    virtual std::vector<sim_mob::RoadSegment*>
    post_fwdBckSegments_t ();
  };

  class RoadSegmentsAt_t_pimpl: public virtual RoadSegmentsAt_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    segmentID (const ::std::string&);

    virtual void
    post_RoadSegmentsAt_t ();
  };

  class segment_t_pimpl: public virtual segment_t_pskel
  {
	  sim_mob::RoadSegment *rs;
    public:
    virtual void
    pre ();

    virtual void
    segmentID (const ::std::string&);

    virtual void
    startingNode (const ::std::string&);

    virtual void
    endingNode (const ::std::string&);

    virtual void
    maxSpeed (short);

    virtual void
    Length (unsigned int);

    virtual void
    Width (unsigned int);

    virtual void
    Lanes (std::vector<sim_mob::Lane*>);

    virtual void
    Obstacles ();

    virtual void
    KurbLine (std::vector<sim_mob::Point2D>);

    virtual sim_mob::RoadSegment*
    post_segment_t ();
  };

  class link_t_pimpl: public virtual link_t_pskel
  {
	  sim_mob::Link * link;
    public:
    virtual void
    pre ();

    virtual void
    linkID (const ::std::string&);

    virtual void
    roadName (const ::std::string&);

    virtual void
    StartingNode (const ::std::string&);

    virtual void
    EndingNode (const ::std::string&);

    virtual void
    Segments (std::pair<std::vector<sim_mob::RoadSegment*>,std::vector<sim_mob::RoadSegment*> >);

    virtual sim_mob::Link*
    post_link_t ();
  };

  class separator_t_pimpl: public virtual separator_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    separator_ID (unsigned short);

    virtual void
    separator_value (bool);

    virtual void
    post_separator_t ();
  };

  class separators_t_pimpl: public virtual separators_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    Separator ();

    virtual void
    post_separators_t ();
  };

  class DomainIsland_t_pimpl: public virtual DomainIsland_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    domainIsland_ID (unsigned short);

    virtual void
    domainIsland_value (bool);

    virtual void
    post_DomainIsland_t ();
  };

  class DomainIslands_t_pimpl: public virtual DomainIslands_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    domainIslands ();

    virtual void
    post_DomainIslands_t ();
  };

  class offset_t_pimpl: public virtual offset_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    offset_ID (unsigned short);

    virtual void
    offset_value (unsigned int);

    virtual void
    post_offset_t ();
  };

  class offsets_t_pimpl: public virtual offsets_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    offset ();

    virtual void
    post_offsets_t ();
  };

  class ChunkLength_t_pimpl: public virtual ChunkLength_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    chunklength_ID (unsigned short);

    virtual void
    chunklength_value (unsigned int);

    virtual void
    post_ChunkLength_t ();
  };

  class ChunkLengths_t_pimpl: public virtual ChunkLengths_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    chunklength ();

    virtual void
    post_ChunkLengths_t ();
  };

  class LanesVector_t_pimpl: public virtual LanesVector_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    laneID (const ::std::string&);

    virtual void
    post_LanesVector_t ();
  };

  class EntranceAngle_t_pimpl: public virtual EntranceAngle_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    entranceAngle_ID (unsigned short);

    virtual void
    entranceAngle_value (unsigned int);

    virtual void
    post_EntranceAngle_t ();
  };

  class EntranceAngles_t_pimpl: public virtual EntranceAngles_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    entranceAngle ();

    virtual void
    post_EntranceAngles_t ();
  };

  class UniNode_t_pimpl: public virtual UniNode_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    nodeID (const ::std::string&);

    virtual void
    location (sim_mob::Point2D);

    virtual void
    Connectors ();

    virtual sim_mob::UniNode*
    post_UniNode_t ();
  };

  class roundabout_t_pimpl: public virtual roundabout_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    nodeID (const ::std::string&);

    virtual void
    location (sim_mob::Point2D);

    virtual void
    roadSegmentsAt ();

    virtual void
    Connectors ();

    virtual void
    ChunkLengths ();

    virtual void
    Offsets ();

    virtual void
    Separators ();

    virtual void
    addDominantLane ();

    virtual void
    roundaboutDominantIslands (float);

    virtual void
    roundaboutNumberOfLanes (int);

    virtual void
    entranceAngles ();

    virtual sim_mob::MultiNode*
    post_roundabout_t ();
  };

  class intersection_t_pimpl: public virtual intersection_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    nodeID (const ::std::string&);

    virtual void
    location (sim_mob::Point2D);

    virtual void
    roadSegmentsAt ();

    virtual void
    Connectors ();

    virtual void
    ChunkLengths ();

    virtual void
    Offsets ();

    virtual void
    Separators ();

    virtual void
    additionalDominantLanes ();

    virtual void
    additionalSubdominantLanes ();

    virtual void
    domainIslands ();

    virtual sim_mob::MultiNode*
    post_intersection_t ();
  };

  class RoadItem_No_Attr_t_pimpl: public virtual RoadItem_No_Attr_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    start (sim_mob::Point2D);

    virtual void
    end (sim_mob::Point2D);

    virtual void
    post_RoadItem_No_Attr_t ();
  };

  class RoadItem_t_pimpl: public virtual RoadItem_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    Offset (unsigned short);

    virtual void
    start (sim_mob::Point2D);

    virtual void
    end (sim_mob::Point2D);

    virtual sim_mob::RoadItem*
    post_RoadItem_t ();
  };

  class BusStop_t_pimpl: public virtual BusStop_t_pskel,
    public ::geo::RoadItem_t_pimpl
  {
    public:
    virtual void
    pre ();

    virtual void
    busStopID (const ::std::string&);

    virtual void
    lane_location (const ::std::string&);

    virtual void
    is_Terminal (bool);

    virtual void
    is_Bay (bool);

    virtual void
    has_shelter (bool);

    virtual void
    busCapacityAsLength (unsigned int);

    virtual void
    post_BusStop_t ();
  };

  class ERP_Gantry_t_pimpl: public virtual ERP_Gantry_t_pskel,
    public ::geo::RoadItem_t_pimpl
  {
    public:
    virtual void
    pre ();

    virtual void
    ERP_GantryID (const ::std::string&);

    virtual void
    post_ERP_Gantry_t ();
  };

  class FormType_pimpl: public virtual FormType_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    TextBox (int);

    virtual void
    TextArea (int);

    virtual void
    Header (int);

    virtual void
    post_FormType ();
  };

  class PointPair_t_pimpl: public virtual PointPair_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    first (sim_mob::Point2D);

    virtual void
    second (sim_mob::Point2D);

    virtual void
    post_PointPair_t ();
  };

  class crossing_t_pimpl: public virtual crossing_t_pskel,
    public ::geo::RoadItem_t_pimpl
  {
	  sim_mob::Crossing *crossing;
    public:
    virtual void
    pre ();

    virtual void
    crossingID (const ::std::string&);

    virtual void
    nearLine ();

    virtual void
    farLine ();

    virtual sim_mob::Crossing*
    post_crossing_t ();
  };

  class RoadBump_t_pimpl: public virtual RoadBump_t_pskel,
    public ::geo::RoadItem_t_pimpl
  {
    public:
    virtual void
    pre ();

    virtual void
    roadBumpID (const ::std::string&);

    virtual void
    segmentID (const ::std::string&);

    virtual void
    post_RoadBump_t ();
  };

  class RoadNetwork_t_pimpl: public virtual RoadNetwork_t_pskel
  {
	  sim_mob::RoadNetwork &rn;
    public:
	  RoadNetwork_t_pimpl();
    virtual void
    pre ();

    virtual void
    Links (std::vector<sim_mob::Link*>);

    virtual void
    Nodes ();

    virtual void
    post_RoadNetwork_t ();
  };

  class RoadItems_t_pimpl: public virtual RoadItems_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    BusStop ();

    virtual void
    ERP_Gantry ();

    virtual void
    Crossing (sim_mob::Crossing*);

    virtual void
    RoadBump ();

    virtual void
    post_RoadItems_t ();
  };

  class GeoSpatial_t_pimpl: public virtual GeoSpatial_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    RoadNetwork ();

    virtual void
    post_GeoSpatial_t ();
  };

  class SimMobility_t_pimpl: public virtual SimMobility_t_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    GeoSpatial ();

    virtual void
    post_SimMobility_t ();
  };

  class Lanes_pimpl: public virtual Lanes_pskel
  {
	  std::vector<sim_mob::Lane*> lanes;
    public:
    virtual void
    pre ();

    virtual void
    Lane (sim_mob::Lane*);

    virtual std::vector<sim_mob::Lane*>
    post_Lanes ();
  };

  class Segments_pimpl: public virtual Segments_pskel
  {
	  std::vector<sim_mob::RoadSegment*> fwd,bck,uniq;
    public:
    virtual void
    pre ();

    virtual void
    FWDSegments (std::vector<sim_mob::RoadSegment*>);

    virtual void
    BKDSegments (std::vector<sim_mob::RoadSegment*>);

    virtual std::pair<std::vector<sim_mob::RoadSegment*>,std::vector<sim_mob::RoadSegment*> >
    post_Segments ();
  };

  class Links_pimpl: public virtual Links_pskel
  {
	  std::vector<sim_mob::Link*> links;
    public:
    virtual void
    pre ();

    virtual void
    Link (sim_mob::Link*);

    virtual std::vector<sim_mob::Link*>
    post_Links ();
  };

  class Nodes_pimpl: public virtual Nodes_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    UniNodes (std::set<sim_mob::UniNode*>);

    virtual void
    Intersections (std::vector<sim_mob::MultiNode*>);

    virtual void
    roundabouts (std::vector<sim_mob::MultiNode*>);

    virtual void
    post_Nodes ();
  };

  class UniNodes_pimpl: public virtual UniNodes_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    UniNode (sim_mob::UniNode*);

    virtual std::set<sim_mob::UniNode*>
    post_UniNodes ();
  };

  class Intersections_pimpl: public virtual Intersections_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    Intersection (sim_mob::MultiNode*);

    virtual std::vector<sim_mob::MultiNode*>
    post_Intersections ();
  };

  class roundabouts_pimpl: public virtual roundabouts_pskel
  {
    public:
    virtual void
    pre ();

    virtual void
    roundabout (sim_mob::MultiNode*);

    virtual std::vector<sim_mob::MultiNode*>
    post_roundabouts ();
  };
}

#endif // GEO8_PIMPL_HPP
