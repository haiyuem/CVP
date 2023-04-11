L2_size_dir=L2size_1M_LHT64entry
dir=logs/${L2_size_dir}
##tag and trace for three workloads
tag_sketch=sketch_10x200
trace_sketch=/home/haiyue/research/value_prediction/Value-Prediction-for-Input-Data-Commonality/sketch/trace_${tag_sketch}
tag_mjpeg=mpeg2enc_1img_libmpeg2encpp
trace_mjpeg=/home/haiyue/research/value_prediction/mjpegtools/image/lane_detection/trace_${tag_mjpeg}
tag_lane=lane_det_ATL_spNBmm_b1
trace_lane=/home/haiyue/research/ultra_fast_lane_detection/lane_detect_convert/caffe_lane_detection/trace_${tag_lane}

##three runs: baseline, perfect, prediction

#sketch
# ./cvp ${trace_sketch} > ${dir}/${tag_sketch}_base.log
# ./cvp -v -p -t 2 ${trace_sketch} > ${dir}/${tag_sketch}_perfect.log
# ./cvp -v -t 2 ${trace_sketch} > ${dir}/${tag_sketch}_predict_lastload.log

#mjpeg
# ./cvp ${trace_mjpeg} > ${dir}/${tag_mjpeg}_base.log
# ./cvp -v -p -t 2 ${trace_mjpeg} > ${dir}/${tag_mjpeg}_perfect.log
# ./cvp -v -t 2 ${trace_mjpeg} > ${dir}/${tag_mjpeg}_predict_lastload.log

#Lane detection
# ./cvp ${trace_lane} > ${dir}/${tag_lane}_base.log
# ./cvp -v -p -t 2 ${trace_lane} > ${dir}/${tag_lane}_perfect.log
./cvp -v -t 2 ${trace_lane} > ${dir}/${tag_lane}_predict_lastload.log