#ifndef RADAR_DISPLAY_H
#define RADAR_DISPLAY_H

#ifndef Q_MOC_RUN
#include <boost/circular_buffer.hpp>

#include <rviz/message_filter_display.h>
#include <rviz/tool.h>

#include <radar_sensor_msgs/RadarData.h>
#endif

namespace Ogre
{
class SceneNode;
}

namespace rviz
{
class ColorProperty;
class FloatProperty;
class IntProperty;
class BoolProperty;
 class ViewportMouseEvent;
}

// All the source in this plugin is in its own namespace.  This is not
// required but is good practice.
namespace rviz_radar_plugin
{

class RadarVisual;

// Here we declare our new subclass of rviz::Display.  Every display
// which can be listed in the "Displays" panel is a subclass of
// rviz::Display.
//
// RadarDisplay will show a 3D arrow showing the direction and magnitude
// of the RADAR acceleration vector.  The base of the arrow will be at
// the frame listed in the header of the Radar message, and the
// direction of the arrow will be relative to the orientation of that
// frame.  It will also optionally show a history of recent
// acceleration vectors, which will be stored in a circular buffer.
//
// The RadarDisplay class itself just implements the circular buffer,
// editable parameters, and Display subclass machinery.  The visuals
// themselves are represented by a separate class, RadarVisual.  The
// idiom for the visuals is that when the objects exist, they appear
// in the scene, and when they are deleted, they disappear.
 class RadarDisplay: public rviz::MessageFilterDisplay<radar_sensor_msgs::RadarData>
 {
Q_OBJECT
public:
  // Constructor.  pluginlib::ClassLoader creates instances by calling
  // the default constructor, so make sure you have one.
  RadarDisplay();
  virtual ~RadarDisplay();

  // Overrides of protected virtual functions from Display.  As much
  // as possible, when Displays are not enabled, they should not be
  // subscribed to incoming data and should not show anything in the
  // 3D view.  These functions are where these connections are made
  // and broken.
protected:
  virtual void onInitialize();

  // A helper to clear this display back to the initial state.
  virtual void reset();

  // These Qt slots get connected to signals indicating changes in the user-editable properties.
private Q_SLOTS:
  void updateColorAndAlpha();
  void updateScale();
  void updateHistoryLength();
  void updateMinRange();
  void updateMaxRange();
  void updateShowRaw();
  void updateShowSpeedArrows();
  void updateShowTargetInfo();
  
  // Function to handle an incoming ROS message.
private:
  void processMessage( const radar_sensor_msgs::RadarData::ConstPtr& msg );

  // Storage for the list of visuals.  It is a circular buffer where
  // data gets popped from the front (oldest) and pushed to the back (newest)
  boost::circular_buffer<boost::shared_ptr<RadarVisual> > visuals_;

  // Booleans to store whether to show targets:
  bool show_raw_;
  
  // User-editable property variables.
  rviz::BoolProperty* show_raw_property_;
  rviz::ColorProperty* color_raw_;
  rviz::FloatProperty* alpha_raw_;
  rviz::FloatProperty* scale_raw_;
  rviz::IntProperty* history_length_property_;
  rviz::FloatProperty* min_range_property_;
  rviz::FloatProperty* max_range_property_;
  rviz::BoolProperty* show_speed_property_;
  rviz::BoolProperty* show_info_property_;
};

} // end namespace rviz_radar_plugin

#endif // RADAR_DISPLAY_H

