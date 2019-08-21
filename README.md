Example compilation: g++ -pthread -I ~/.local/lib/include/ -std=c++17 add_2_batch.cpp -o add_2 -L ~/.local/lib/lib -lseal -O3
This path may change depending on where SEAL is installed in your system.

To profile: ./profile.sh ./add_2 power_add_2.txt time_add_2.txt
To make profile.sh executable: chmod +x profile.sh
May need to install calc if not already installed.
