% -------------------------------------------------------------------------
% Sensing and IOT project - 10th January 2019
% -------------------------------------------------------------------------
% Script that performs time series analysis for colour senor data of a
% leaf and humidity data from an API.
% -------------------------------------------------------------------------clear
close all
clc

% Load data
Data = load('NodeMCU2.csv');

% Find missing values
Data_Hrgb = Data;
Idx_missing = Data_Hrgb == 0;

Data_Hrgb(Idx_missing) = NaN;

% Impute missing values
Data_Hrgb_filled = fillmissing(Data_Hrgb,'linear');

% Find R G B ratios
Sum_RGB = sum(Data_Hrgb_filled(:,2:4),2);
R = round(Data_Hrgb_filled(:,2)./Sum_RGB*255)/255;
G = round(Data_Hrgb_filled(:,3)./Sum_RGB*255)/255;
B = round(Data_Hrgb_filled(:,4)./Sum_RGB*255)/255;

% Humidity data
Humidity = Data_Hrgb_filled(:,1);

% Time
TT_start = datetime(2018,12,23,20,00,00);
TT_end = datetime(2018,12,31,17,00,00);
TT = TT_start:hours(1):TT_end;

% Data plots
% RGB plots
figure
plot(TT,R,'r-')
hold on
plot(TT,G,'g-')
plot(TT,B,'b-')
hold off
title('RGB plot')
xlabel('Date Time')
ylabel('R, G, B value')
legend('R','G','B')

% Humidity plot
figure
plot(TT,Humidity)
title('Humidity plot')
xlabel('Time')
ylabel('Humidity percentage')

% Normalised plots of RGB and Humidity
figure
plot(TT,(R - mean(R))/std(R),'r-')
hold on
plot(TT,(G - mean(G))/std(G),'g-')
plot(TT,(B - mean(B))/std(B),'b-')
plot(TT,(Humidity - mean(Humidity))/std(Humidity),'k-')
hold off
title('Normalised RGB and humidity plot')
xlabel('Date Time')
ylabel('R, G, B, Humidity values')
legend('R','G','B','Humidity')

% Colour gradient over time
figure
for ii = 1:length(R)
    H = histogram(ii,1);
    H.FaceColor = [R(ii) G(ii) B(ii)];
    H.EdgeColor = 'none';
    H.FaceAlpha = 1;
    hold on
end
hold off
title('Colour gradient over time')
xlabel('Time step')
ylabel('Colour')
axis([0 190 0 1])

%% Determine correlation if any of colour and humidity
% Combined colour
RGB = [R*255 G*255 B*255];
combined_rgb = 65536*RGB(:,1) + 256*RGB(:,2) + RGB(:,3);
% Evaluating correlation 
[rho, pp] = corrcoef(combined_rgb,Humidity);

% There is a slight negative correlation between colour and humidity. 
% The correlation coefficient is significant at the 5% level. 
% However, this could be spurious and was not evident in any plots. 
% Correlation does not imply causition! 

%% For forecasting

% The RGB data does not seem to have any seasonal/cyclical variation but
% only show trends.
% The humidity data does not seem to show marked trend or seasonal
% variation.

% Assessing stationarity of time series using ACF and PACF plots 
figure
subplot(2,1,1)
autocorr(R)
title('R ratio ACF')
subplot(2,1,2)
parcorr(R)
title('R ratio PACF')

figure
subplot(2,1,1)
autocorr(G)
title('G ratio ACF')
subplot(2,1,2)
parcorr(G)
title('G ratio PACF')

figure
subplot(2,1,1)
autocorr(B)
title('B ratio ACF')
subplot(2,1,2)
parcorr(B)
title('B ratio PACF')

figure
subplot(2,1,1)
autocorr(Humidity)
subplot(2,1,2)
parcorr(Humidity)
title('Humidity ACF and PACF')

% ACF curves for all variables gradually decrease from a positive value 
% which shows that the series are not stationary.

% PACF after the first 2 lags are not significant and hence we use AR
% models with lag 2 for each series.

% Fitting ARIMA models to data 
Mdl_rgb = arima(2,0,0);
EstmdlR = estimate(Mdl_rgb,R);
EstmdlG = estimate(Mdl_rgb,G);
EstmdlB = estimate(Mdl_rgb,B);
EstmdlH = estimate(Mdl_rgb,Humidity);

% forecasting and plots  
% R 
[Rf,Rmse] = forecast(EstmdlR,24);
figure
plot(TT,R,'r-')
hold on
plot(TT(end):hours(1):datetime(2019,01,01,16,00,00),Rf,'r.-')
hold off 
xlabel('Time')
ylabel('R ratios')
legend('Data','Forecast')
title('R ratio: data and forecasted values')

% G 
[Gf,Gmse] = forecast(EstmdlG,24);
figure
plot(TT,G,'g-')
hold on
plot(TT(end):hours(1):datetime(2019,01,01,16,00,00),Gf,'g.-')
hold off 
xlabel('Time')
ylabel('G ratios')
legend('Data','Forecast')
title('G ratio: data and forecasted values')

% B 
[Bf,Bmse] = forecast(EstmdlB,24);
figure
plot(TT,B,'b-')
hold on
plot(TT(end):hours(1):datetime(2019,01,01,16,00,00),Bf,'b.-')
hold off 
xlabel('Time')
ylabel('B ratios')
legend('Data','Forecast')
title('B ratio: data and forecasted values')

