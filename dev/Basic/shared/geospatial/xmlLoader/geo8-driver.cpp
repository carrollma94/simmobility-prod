// Not copyrighted - public domain.
//
// This sample parser implementation was generated by CodeSynthesis XSD,
// an XML Schema to C++ data binding compiler. You may use it in your
// programs without any restrictions.
//

#include "geo8-pimpl.hpp"

#include <iostream>
#include "main1.hpp"

int
main1 (int argc, char* argv[])
{
	std::cout << "In main1\n";

  try
  {
    // Instantiate individual parsers.
    //
    ::geo::SimMobility_t_pimpl SimMobility_t_p;
    ::geo::GeoSpatial_t_pimpl GeoSpatial_t_p;
    ::geo::RoadNetwork_t_pimpl RoadNetwork_t_p;
    ::geo::Links_pimpl Links_p;
    ::geo::link_t_pimpl link_t_p;
    ::xml_schema::string_pimpl string_p;
    ::geo::Segments_pimpl Segments_p;
    ::geo::fwdBckSegments_t_pimpl fwdBckSegments_t_p;
    ::geo::segment_t_pimpl segment_t_p;
    ::xml_schema::short_pimpl short_p;
    ::xml_schema::unsigned_int_pimpl unsigned_int_p;
    ::geo::Lanes_pimpl Lanes_p;
    ::geo::lane_t_pimpl lane_t_p;
    ::xml_schema::boolean_pimpl boolean_p;
    ::geo::PolyLine_t_pimpl PolyLine_t_p;
    ::geo::PolyPoint_t_pimpl PolyPoint_t_p;
    ::geo::Point2D_t_pimpl Point2D_t_p;
    ::geo::RoadItems_t_pimpl RoadItems_t_p;
    ::geo::BusStop_t_pimpl BusStop_t_p;
    ::xml_schema::unsigned_short_pimpl unsigned_short_p;
    ::geo::ERP_Gantry_t_pimpl ERP_Gantry_t_p;
    ::geo::crossing_t_pimpl crossing_t_p;
    ::geo::PointPair_t_pimpl PointPair_t_p;
    ::geo::RoadBump_t_pimpl RoadBump_t_p;
    ::geo::Nodes_pimpl Nodes_p;
    ::geo::UniNodes_pimpl UniNodes_p;
    ::geo::UniNode_t_pimpl UniNode_t_p;
    ::geo::connectors_t_pimpl connectors_t_p;
    ::geo::connector_t_pimpl connector_t_p;
    ::geo::Intersections_pimpl Intersections_p;
    ::geo::intersection_t_pimpl intersection_t_p;
    ::geo::RoadSegmentsAt_t_pimpl RoadSegmentsAt_t_p;
    ::geo::ChunkLengths_t_pimpl ChunkLengths_t_p;
    ::geo::ChunkLength_t_pimpl ChunkLength_t_p;
    ::geo::offsets_t_pimpl offsets_t_p;
    ::geo::offset_t_pimpl offset_t_p;
    ::geo::separators_t_pimpl separators_t_p;
    ::geo::separator_t_pimpl separator_t_p;
    ::geo::LanesVector_t_pimpl LanesVector_t_p;
    ::geo::DomainIslands_t_pimpl DomainIslands_t_p;
    ::geo::DomainIsland_t_pimpl DomainIsland_t_p;
    ::geo::roundabouts_pimpl roundabouts_p;
    ::geo::roundabout_t_pimpl roundabout_t_p;
    ::xml_schema::float_pimpl float_p;
    ::xml_schema::int_pimpl int_p;
    ::geo::EntranceAngles_t_pimpl EntranceAngles_t_p;
    ::geo::EntranceAngle_t_pimpl EntranceAngle_t_p;

    // Connect the parsers together.
    //
    SimMobility_t_p.parsers (GeoSpatial_t_p);

    GeoSpatial_t_p.parsers (RoadNetwork_t_p);

    RoadNetwork_t_p.parsers (Links_p,
                             Nodes_p);

    Links_p.parsers (link_t_p);

    link_t_p.parsers (string_p,
                      string_p,
                      string_p,
                      string_p,
                      Segments_p);

    Segments_p.parsers (fwdBckSegments_t_p,
                        fwdBckSegments_t_p);

    fwdBckSegments_t_p.parsers (segment_t_p);

    segment_t_p.parsers (string_p,
                         string_p,
                         string_p,
                         short_p,
                         unsigned_int_p,
                         unsigned_int_p,
                         Lanes_p,
                         RoadItems_t_p,
                         PolyLine_t_p);

    Lanes_p.parsers (lane_t_p);

    lane_t_p.parsers (string_p,
                      unsigned_int_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      boolean_p,
                      PolyLine_t_p);

    PolyLine_t_p.parsers (PolyPoint_t_p);

    PolyPoint_t_p.parsers (string_p,
                           Point2D_t_p);

    Point2D_t_p.parsers (unsigned_int_p,
                         unsigned_int_p);

    RoadItems_t_p.parsers (BusStop_t_p,
                           ERP_Gantry_t_p,
                           crossing_t_p,
                           RoadBump_t_p);

    BusStop_t_p.parsers (unsigned_short_p,
                         Point2D_t_p,
                         Point2D_t_p,
                         string_p,
                         string_p,
                         boolean_p,
                         boolean_p,
                         boolean_p,
                         unsigned_int_p);

    ERP_Gantry_t_p.parsers (unsigned_short_p,
                            Point2D_t_p,
                            Point2D_t_p,
                            string_p);

    crossing_t_p.parsers (unsigned_short_p,
                          Point2D_t_p,
                          Point2D_t_p,
                          string_p,
                          PointPair_t_p,
                          PointPair_t_p);

    PointPair_t_p.parsers (Point2D_t_p,
                           Point2D_t_p);

    RoadBump_t_p.parsers (unsigned_short_p,
                          Point2D_t_p,
                          Point2D_t_p,
                          string_p,
                          string_p);

    Nodes_p.parsers (UniNodes_p,
                     Intersections_p,
                     roundabouts_p);

    UniNodes_p.parsers (UniNode_t_p);

    UniNode_t_p.parsers (string_p,
                         Point2D_t_p,
                         connectors_t_p);

    connectors_t_p.parsers (connector_t_p);

    connector_t_p.parsers (string_p,
                           string_p);

    Intersections_p.parsers (intersection_t_p);

    intersection_t_p.parsers (string_p,
                              Point2D_t_p,
                              RoadSegmentsAt_t_p,
                              connectors_t_p,
                              ChunkLengths_t_p,
                              offsets_t_p,
                              separators_t_p,
                              LanesVector_t_p,
                              LanesVector_t_p,
                              DomainIslands_t_p);

    RoadSegmentsAt_t_p.parsers (string_p);

    ChunkLengths_t_p.parsers (ChunkLength_t_p);

    ChunkLength_t_p.parsers (unsigned_short_p,
                             unsigned_int_p);

    offsets_t_p.parsers (offset_t_p);

    offset_t_p.parsers (unsigned_short_p,
                        unsigned_int_p);

    separators_t_p.parsers (separator_t_p);

    separator_t_p.parsers (unsigned_short_p,
                           boolean_p);

    LanesVector_t_p.parsers (string_p);

    DomainIslands_t_p.parsers (DomainIsland_t_p);

    DomainIsland_t_p.parsers (unsigned_short_p,
                              boolean_p);

    roundabouts_p.parsers (roundabout_t_p);

    roundabout_t_p.parsers (string_p,
                            Point2D_t_p,
                            RoadSegmentsAt_t_p,
                            connectors_t_p,
                            ChunkLengths_t_p,
                            offsets_t_p,
                            separators_t_p,
                            LanesVector_t_p,
                            float_p,
                            int_p,
                            EntranceAngles_t_p);

    EntranceAngles_t_p.parsers (EntranceAngle_t_p);

    EntranceAngle_t_p.parsers (unsigned_short_p,
                               unsigned_int_p);

    // Parse the XML document.
    //
    ::xml_schema::document doc_p (
      SimMobility_t_p,
      "http://www.smart.mit.edu/geo",
      "SimMobility");
    SimMobility_t_p.pre ();
    std::cout << "In main1 SimMobility_t_p.pre() done\n";
    doc_p.parse ("data/XML_OutPut.xml");
    std::cout << "In main1 parse done\n";
    SimMobility_t_p.post_SimMobility_t ();
    std::cout << "In main1 post done\n";
  }
  catch (const ::xml_schema::exception& e)
  {
	  std::cout << "main1 failed\n";
    std::cerr << e << std::endl;
    return 1;
  }
  catch (const std::ios_base::failure&)
  {
	  std::cout << "main1 failed2\n";
    std::cerr << ": error: io failure" << std::endl;
    return 1;
  }
}

