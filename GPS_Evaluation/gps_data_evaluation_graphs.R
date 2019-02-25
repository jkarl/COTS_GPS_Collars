# Prep and Plot GPS Data

library(ggplot2)
library(rgdal)
library(dplyr)

path <- "C:\\Users\\Jason Karl\\Documents\\GitHub\\COTS_GPS_Collars\\COTS_GPS_Collars\\GPS_Evaluation"
file <- "GPSLOG_5min_Indoor.20180314.csv" #"GPSLOG_5min_Outdoor.20180313.csv"
attr.names <- c("HDOP","Lat","Lon","NumSats","GPSDate","GPSTime","FixTime","BestTime","EndTime")
plot.title <- "5-min Indoor"

# Load data and assign names
gps.data <- read.csv(paste(path,file,sep="\\"),header=T,stringsAsFactors=F)
names(gps.data) <- attr.names

# Reproject data to UTM projection
coordinates(gps.data) <- ~Lon+Lat
proj4string(gps.data) <- CRS("+proj=longlat +datum=WGS84")
gps.data.u11 <- spTransform(gps.data, CRS=CRS("+proj=utm +zone=11 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"))

# Calc difference from average coordinates
meanLat <- mean(gps.data.u11$Lat)
meanLon <- mean(gps.data.u11$Lon)
sdLat <- sd(gps.data.u11$Lat)
sdLon <- sd(gps.data.u11$Lon)

gps.data.u11$diffLat <- meanLat - gps.data.u11$Lat
gps.data.u11$diffLon <- meanLon - gps.data.u11$Lon
gps.data.u11$diffdist <- sqrt(gps.data.u11$diffLat^2+gps.data.u11$diffLon^2)

# Construct plots
plot.df <- data.frame(gps.data.u11)

g1 <- ggplot(data=plot.df, aes(x=FixTime/1000))+geom_histogram(binwidth=5)+
  geom_vline(xintercept = quantile(plot.df$FixTime/1000, 0.9))+
  labs(title=paste("Time to First Fix",title,sep=" - "), x="Seconds", y="# of Fixes")
g1

g2 <- ggplot(data=plot.df, aes(x=BestTime/1000))+geom_histogram(binwidth=5)+
  geom_vline(xintercept = quantile(plot.df$BestTime/1000, 0.9))+
  labs(title=paste("Time to Best Fix",title,sep=" - "), x="Seconds", y="# of Fixes")
g2


g <- ggplot(data=plot.df[plot.df$HDOP<500,], aes(x=diffLon,y=diffLat,color=HDOP))+geom_point()+
  scale_color_gradient2(low="#006837", mid="#fee08b", high="#a50026", midpoint=200)+
  stat_ellipse(level=0.9)+geom_hline(yintercept = 0) + geom_vline(xintercept = 0)+
  labs(title=paste("GPS positional error",title,sep=" - "), x="Difference from Avg Longitude (m)", y="Difference from Avg Latitude (m)")
g

