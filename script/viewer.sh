#!bin/sh

rsim memory image img

rsim module image_reader reader

rsim connect reader image img

rsim set reader path "D:\slam_data\data_odometry_gray\dataset\sequences\00\image_0"
rsim set reader image_name "%06d.png"
rsim set reader print n
rsim set reader rev_rows y
rsim set reader cf 30

rsim module slam_viewer viewer

rsim connect viewer image img

rsim set viewer cf 30
rsim set viewer print n

rsim run

