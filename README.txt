--------------------------------------------------
Sensing and IOT project - 10th January 2019
--------------------------------------------------
README file
--------------------------------------------------
Project Objectives:
--------------------------------------------------

Identify correlations between relative humidity and colour of a leaf;
Utilise two different data collection sources (different nature), sampled at the appropriate rate and stored in an appropriate manner;
Develop a prototype for an application that can suggest when to water a plant based on the colour of the leaf;

Folder Structure (code):
--------------------------------------------------
Googlesheetapp.txt - Asigns variables of humidity, r, g, and b to columns in the google sheet and creates a script ID when 
app is deployed

plant_tracker_gui.m - Visualises the status of the plant, whether it needs to be water, based on the colour of the leaf. 
To start, press the "Load & Process" button. You can drag the slider anywhere on the x-axis, and press the button again, 
the plant health monitor might change and the "NOW" colour will change. 

Sensing.INO - The arduino code used for the sensing of both the weather and colour information. 

TS_analysis.m - Performs the basic time series analysis, and fing the correlation coefficient. Generates plots for autocorrelation 
and regression as well to understand its behaviour in the next 24 hours. 

NodeMCU.csv - The data file, which was stored in a Google Sheet, and updated for 9 days. 

Plots:
--------------------------------------------------
plot1.png - RGB vs. Time

plot2.png - Humidity vs. Time

plot3.png - Normalised RGB and Humidity vs.Time
 
plot4.png - Colour gradient over time

plot5.png - G ratio ACF and PACF

plot6.png - R ratio ACF and PACF

plot7.png - B ratio ACF and PACF

plot8.png - Sample Autocorrelation function and Humdity ACF and PACF

plot9.png - R ratio: Data and forecast value

plot10.png - G ratio: Data and forecast value

plot11.png - B ratio: Data and forecast value


Notes of Processing Time:
---------------------------------------------------
TS_analysis.m runs within 5 seconds
plant_tracker_gui.m will take approximately 10 seconds to run 


Spicification - Intel Core i7-7500U CPU @2.7 GHz, 16GB RAM
