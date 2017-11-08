# -*- coding: utf-8 -*-

import os
import os.path

for root, dirs, files in os.walk("C:\\Users\\liuwei64\\Desktop\\speex-1.2beta3\\libspeex"):
	for name in files:
		print ("libspeex/" + name + " \\");