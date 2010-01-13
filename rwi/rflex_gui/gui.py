#!/usr/bin/python

import roslib; roslib.load_manifest('rflex_gui')
import rospy
import wx

from std_msgs.msg import Bool
from std_msgs.msg import Float32

#    pub = rospy.Publisher('chatter', String)
#        pub.publish(String(str))

class RflexGui(wx.Frame):
	def __init__(self, parent, id, title):
		self.sonar_state = False
		self.brake_state = False
		self.volts = 0.0
	
		wx.Frame.__init__(self, parent, id, title, (-1, -1));
		panel = wx.Panel(self, wx.ID_ANY);
		box = wx.BoxSizer(wx.VERTICAL)
		
		### Buttons ###
		self.sonar = wx.Button(panel, 1, 'Sonar')
		self.brake = wx.Button(panel, 2, 'Brake')
		
		### Text ###
		self.voltage = wx.StaticText(panel, 3, 'Battery Voltage: 00.0V')

		wx.EVT_BUTTON(self, 1, self.change_sonar_state)
		wx.EVT_BUTTON(self, 2, self.change_brake_state)

		box.Add(self.sonar, 0, wx.EXPAND)
		box.Add(self.brake, 0, wx.EXPAND)
		box.Add(self.voltage, 0, wx.ALL, 12)
		
		panel.SetSizer(box)
		self.Center()
		
		self.sonar_pub = rospy.Publisher('cmd_sonar_power', Bool)
		self.brake_pub = rospy.Publisher('cmd_brake_power', Bool)
		
		box.Fit(self)
		
	def change_sonar_state(self, event):
		rospy.loginfo("Changing sonar state to %s" % (not self.sonar_state))
		self.sonar_pub.publish(not self.sonar_state)
		
	def change_brake_state(self, event):
		rospy.loginfo("Changing brake state to %s" % (not self.brake_state))
		self.brake_pub.publish(not self.brake_state)

	def onsonar(self, data):
		if(self.sonar_state != data.data):
			self.sonar_state = data.data
			if data.data:
				self.sonar.SetLabel('Sonar: ON')
				self.sonar.SetBackgroundColour('GREEN')
			else:
				self.sonar.SetLabel('Sonar: OFF')
				self.sonar.SetBackgroundColour('RED')
			rospy.loginfo("Sonar is %s" % ("on" if data.data else "off"))

	def onbrake(self, data):
		if(self.brake_state != data.data):
			self.brake_state = data.data
			if data.data:
				self.brake.SetLabel('Brake: ON')
				self.brake.SetBackgroundColour("RED")
			else:
				self.brake.SetLabel('Brake: OFF')
				self.brake.SetBackgroundColour("GREEN")
			
			rospy.loginfo("Brake is %s" % ("on" if data.data else "off"))
		
	def onvoltage(self, data):
		if(self.volts != data.data):
			self.volts = data.data;
			self.voltage.SetLabel('Battery Voltage: %sV' % data.data)
			if data.data >= 24:
				self.voltage.SetForegroundColour('GREEN')
			elif data.data >= 20:
				self.voltage.SetForegroundColour('ORANGE')
			else:
				self.voltage.SetForegroundColour('RED')

if __name__ == '__main__':
	try:
		app = wx.App()
		gui = RflexGui(None, -1, "rflex status utility")
		gui.Show()

		rospy.init_node('rflex_gui')
		rospy.Subscriber("sonar_power", Bool, gui.onsonar)
		rospy.Subscriber("brake_power", Bool, gui.onbrake)
		rospy.Subscriber("voltage", Float32, gui.onvoltage)
		
		app.MainLoop()
	except rospy.ROSInterruptException: pass
