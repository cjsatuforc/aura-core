// waypoint.hxx -- Provides a class to manage waypoints
//
// Written by Curtis Olson, started September 2000.
//
// Copyright (C) 2000  Curtis L. Olson  - curt@hfrl.umn.edu
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


#ifndef _WAYPOINT_HXX
#define _WAYPOINT_HXX


#ifndef __cplusplus
# error This library requires C++
#endif


#include "python/pyprops.hxx"

#include <string>
using std::string;


/**
 * A class to manage waypoints.
 */

class SGWayPoint {

public:

    /**
     * Waypoint mode.
     * <li> ABSOLUTE: coordinates are longitude, latitude (deg)
     * <li> RELATIVE: coordinates are distance (m), heading (deg)
     */
    enum modetype { 
	ABSOLUTE = 0,
	RELATIVE = 1
    };

private:

    modetype mode;

    double target_lon;
    double target_lat;
    double target_alt_m;
    double target_agl_m;
    double target_speed_kt;
    double target_bank_deg;

    // relative waypoints will have their actual target_lon & lat
    // computed as an offset angle and distance from a specified
    // reference location and heading.  These values should be set to
    // zero to indicate this waypoint is absolute and the target_lon &
    // lat should not ever be updated.
    double offset_hdg_deg;
    double offset_dist_m;

    double distance;

    string id;

public:

    /**
     * Construct a waypoint
     * @param field1 target longitude deg if mode == ABSOLUTE
     * @param field2 target latitude deg if mode == ABSOLUTE
     * @param field1 (offset heading deg if mode == RELATIVE
     * @param dist_m (offset distance m if mode == RELATIVE
     * @param alt_m target altitude
     * @param speed_kt target altitude
     * @param mode type of coordinates/math to use
     * @param s waypoint identifier
     */
    SGWayPoint( const double field1, const double field2,
		const double alt_m = -9999.9, const double agl_m = -9999.9,
                const double speed_kt = 0.0,
		const double bank_deg = 0.0,
                const modetype m = ABSOLUTE,
		const string& s = "" );

    /**
     * Construct a waypoint from a property sub tree
     * @param node a pointer to a property subtree with configuration values
     */
    SGWayPoint( pyPropertyNode *config_node );

    /**
     * Construct a null waypoint
     */
    SGWayPoint();

    /** Destructor */
    ~SGWayPoint();

    /**
     * Calculate course and distances.  For WGS84 and SPHERICAL
     * coordinates lat, lon, and course are in degrees, alt and
     * distance are in meters.
     * @param cur_lon (in) current longitude
     * @param cur_lat (in) current latitude
     * @param cur_alt (in) current altitude
     * @param course (out) heading from current location to this waypoint
     * @param dist (out) distance from current location to this waypoint
     */
    void CourseAndDistance( const double cur_lon, const double cur_lat,
			    const double cur_alt,
			    double *course, double *dist ) const;

    /**
     * Calculate course and distances between a specified starting waypoint
     * and this waypoint.
     * @param wp (in) original waypoint
     * @param course (out) heading from current location to this waypoint
     * @param dist (out) distance from current location to this waypoint
     */
    void CourseAndDistance( const SGWayPoint &wp,
			    double *course, double *dist ) const;

    /**
     * Update the target_lon and target_lat values of this waypoint
     * based on this waypoint's offset heading and offset distance
     * values.  The new target location is computed relative to the
     * provided reference point and reference heading.
     * @param ref the reference waypoint
     * @param ref_heading the reference heading/course
     */
    void update_relative_pos( const SGWayPoint &ref,
                              const double ref_heading_deg );

    /** @return waypoint mode */
    inline modetype get_mode() const { return mode; }

    /** @return waypoint longitude */
    inline double get_target_lon() const { return target_lon; }

    /** @return waypoint latitude */
    inline double get_target_lat() const { return target_lat; }

    /** @return waypoint altitude */
    inline double get_target_alt_m() const { return target_alt_m; }

    /** @return waypoint altitude relative to ground altitude */
    inline double get_target_agl_m() const { return target_agl_m; }

    /** @return waypoint speed */
    inline double get_target_speed_kt() const { return target_speed_kt; }
 
    /** @return waypoint bank angle */
    inline double get_target_bank_deg() const { return target_bank_deg; }
 
    /** @return offset heading */
    inline double get_offset_hdg_deg() const { return offset_hdg_deg; }
 
    /** @return offset heading */
    inline double get_offset_dist_m() const { return offset_dist_m; }

    /**
     * This value is not calculated by this class.  It is simply a
     * placeholder for the user to store a distance value.  This is useful
     * when you stack waypoints together into a route.  You can calculate the
     * distance from the previous waypoint once and save it here.  Adding up
     * all the distances here plus the distance to the first waypoint gives you
     * the total route length.  Note, you must update this value yourself, this
     * is for your convenience only.
     * @return waypoint distance holder (what ever the user has stashed here)
     */
    inline double get_distance() const { return distance; }

    /**
     * Set the waypoint distance value to a value of our choice.
     * @param d distance 
     */
    inline void set_distance( double d ) { distance = d; }

    /** @return waypoint id */
    inline const string& get_id() const { return id; }
};


#endif // _WAYPOINT_HXX
