compile openssl


./config -fPIC -shared -lrt --prefix=/usr

make

sudo make install_sw



To calculate accuracy

run 

./run_error_rate.sh


to generate final sample_data with m,r,s,key

run

./run_gen.sh
