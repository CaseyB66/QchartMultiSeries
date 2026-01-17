# QchartMultiSeries
Demostration of both mutliple QCharts in one window, and multiple series in one QChart.
Uses Qt ver 6.10. I created this in the Qt Creator IDE, the main window design is in a .ui file.
The window opens with two empty charts in a column. Click Go to see simulated data acquisition.
Top chart shows simple flow vs volume loops, bottom chart shows similar loops, plus a triangle wave.
The loops 'drift' in the horizontal direction, and the triangle wave grows in amplitude.
The data are somewhat silly, but allows me to show how to dynamically change the ranges on 
the horizontal and vertical axis.
Data being displayed are in a circular buffer, allowing me to limit how much data the 
QCharts need to handle. Click Stop to see all the data. Click Clear to start over.
