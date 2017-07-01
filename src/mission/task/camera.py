import math

import sys
sys.path.append('/usr/local/lib')
import nav.wgs84

from props import root, getNode

import comms.events
from task import Task

d2r = math.pi / 180.0

class Camera(Task):
    def __init__(self, config_node):
        Task.__init__(self)
        self.imu_node = getNode("/sensors/imu", True)
        self.pos_node = getNode("/position", True)
        self.act_node = getNode("/actuators", True)
        self.task_node = getNode("/task", True)
        self.comms_node = getNode( '/comms', True)
        self.name = config_node.getString("name")
        self.nickname = config_node.getString("nickname")
        self.start_time = 0.0
        if config_node.hasChild("trigger"):
            self.trigger_name = config_node.getString("trigger")
        else:
            self.trigger_name = "channel5"
        if config_node.hasChild("forward_fov_deg"):
            self.forward_fov_deg = config_node.getFloat("forward_fov_deg")
            if self.forward_fov_deg < 10:  self.forward_fov_deg = 10
            if self.forward_fov_deg > 170: self.forward_fov_deg = 170
        else:
            self.forward_fov_deg = 60
        self.fov2_tan = math.tan(self.forward_fov_deg*0.5 * d2r)
        if config_node.hasChild("lateral_fov_deg"):
            self.lateral_fov_deg = config_node.getFloat("lateral_fov_deg")
            if self.lateral_fov_deg < 10:  self.lateral_fov_deg = 10
            if self.lateral_fov_deg > 170: self.lateral_fov_deg = 170
        else:
            self.lateral_fov_deg = 60
        if config_node.hasChild("overlap"):
            self.overlap = config_node.getFloat("overlap")
            if self.overlap < 0:  self.overlap = 0
            if self.overlap > 1: self.overlap = 1
        else:
            self.overlap = 0.7
        self.min_interval = 1.0
        self.max_interval = 10.0
        self.trigger_state = False
        self.trigger_time = 0.0
        self.last_lat = 0.0
        self.last_lon = 0.0
        
    def activate(self):
        self.active = True
    
    def update(self, dt):
        if not self.active:
            return False

        cur_time = self.imu_node.getFloat("timestamp")
        force_trigger = False
        if self.trigger_state:
            if cur_time > self.trigger_time + 0.1:
                # release trigger
                self.trigger_state = False
                self.act_node.setFloat(self.trigger_name, 0.0)
            return True
        else:
            if cur_time < self.trigger_time + self.min_interval:
                # min interval not yet elapsed
                return True
            elif cur_time >= self.trigger_time + self.max_interval:
                # print " max interval force trigger"
                force_trigger = True

        # compute course and distance from previous trigger
        pos_lon = self.pos_node.getFloat("longitude_deg")
        pos_lat = self.pos_node.getFloat("latitude_deg")
        (course_deg, reverse_deg, dist_m) = \
                nav.wgs84.geo_inverse_wgs84( self.last_lat, self.last_lon,
                                             pos_lat, pos_lon )
        agl = self.pos_node.getFloat('altitude_agl_m')
        thresh_dist_m = 2 * self.fov2_tan * agl * (1.0 - self.overlap)
        if dist_m >= thresh_dist_m and self.task_node.getBool('is_airborne'):
            # if we are flying and have moved far enough
            # print " distance based trigger:", thresh_dist_m, dist_m
            force_trigger = True

        if force_trigger:
            self.last_lat = self.pos_node.getFloat('latitude_deg')
            self.last_lon = self.pos_node.getFloat('longitude_deg')
            self.trigger_time = cur_time
            self.trigger_state = True
            self.act_node.setFloat(self.trigger_name, 0.68)
            comms.events.log("camera", "%.8f %.8f %.1f" % (self.last_lat, self.last_lon, self.pos_node.getFloat('altitude_m')))
            if self.comms_node.getBool('display_on'):
                print "camera: %.1f %.8f %.8f %.1f" % (cur_time, self.last_lat, self.last_lon, self.pos_node.getFloat('altitude_m'))
        
    def is_complete(self):
        return False
    
    def close(self):
        self.active = False
        return True
