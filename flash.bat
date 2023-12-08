pyocd erase -c -t hk32f030mf4p6 --config ./pyocd.yaml
pyocd load ./Project/MDK5/Objects/LCM_Community_Build.hex -t hk32f030mf4p6 --config ./pyocd.yaml