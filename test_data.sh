#!/bin/bash

PORT=/dev/ttyACM0

stty cs8 115200 -ixon -crtscts <${PORT}

echo "rx 00cd217b17ff30ae641f6e0003ce7000" >${PORT}
sleep 0.2
echo "rx 00cd207b17ff30ae641f6e0003ce7100" >${PORT}
sleep 0.2
echo "rx 00cd1e7b17ff30ae641f6e0003ce7200" >${PORT}
sleep 0.2
echo "rx 00cd1d7b17ff30ae641f6e0003ce7300" >${PORT}
sleep 0.2
echo "rx 00cd1d7b17ff2fae641f6e0003ce7400" >${PORT}
sleep 0.2
echo "rx 00cd1b7b17ff2fae641f6e0003ce7500" >${PORT}
sleep 0.2
echo "rx 00cd1b7b17ff2fae641f6e0003ce7600" >${PORT}
sleep 0.2
echo "rx 00cd1b7b17ff2fae641f6e0003ce7700" >${PORT}
sleep 0.2
echo "rx 00cd1b7b17ff2fae641f6e0003ce7800" >${PORT}
sleep 0.2
echo "rx 00cd1b7b17ff30ae641f6e0003ce7900" >${PORT}
sleep 0.2
echo "rx 00cd197b17ff2fae641f6e0003ce7a00" >${PORT}
sleep 0.2

## state should now be Ready

echo "rx 0087893d17ff4115631fd1007bcebd05" >${PORT}
sleep 3

## state should now Coast to Apogee

echo "rx 00cc11a417fff06b641f000523ceed05" >${PORT}
sleep 0.2
echo "rx 00cdf7ac17ffae7a641f11050ecef305" >${PORT}
sleep 1
echo "rx 00cda2b817ff2288641fe80409ce0006" >${PORT}
sleep 1

## state should be be Apogee

echo "rx 00cddcc317ff5490641fad0407ce0e06" >${PORT}
sleep 1

## state should now be Descent


echo "rx 00cd491b18ffffaf641f520203ce9906" >${PORT}
sleep 1
echo "rx 00cd444718ff31b5641f3c0103cedf06" >${PORT}
sleep 0.2
echo "rx 00cd664818ff7eb5641f340103cee006" >${PORT}
sleep 0.2
echo "rx 00cdfd4818ff87b5641f300103cee106" >${PORT}
sleep 0.2
echo "rx 00cd914918ff8bb5641f2c0103cee206" >${PORT}
sleep 0.2
echo "rx 00cdfe4918ff7db5641f280103cee306" >${PORT}
sleep 0.2
echo "rx 00cdfe4918ff7db5641f280103cee406" >${PORT}
sleep 0.2

## state should now be Main Chute Deployment

echo "rx 00cd508118ff38b0641f7c0003ce3f07" >${PORT}
sleep 0.2
echo "rx 00cd508118ff38b0641f7c0003ce4007" >${PORT}
sleep 0.2
echo "rx 00cdf58218ff01b0641f790003ce4107" >${PORT}
sleep 0.2
echo "rx 00cdb78318ffe5af641f770003ce4207" >${PORT}
sleep 0.2
echo "rx 00cdb78318ffe5af641f770003ce4307" >${PORT}
sleep 0.2
echo "rx 00cd268518ffa8af641f740003ce4407" >${PORT}
sleep 0.2
echo "rx 00cdf28518ff90af641f720003ce4507" >${PORT}
sleep 0.2
echo "rx 00cdc08618ff8aaf641f700003ce4607" >${PORT}
sleep 0.2
echo "rx 00cd828718ff98af641f6e0003ce4707" >${PORT}
sleep 0.2
echo "rx 00cd828718ff98af641f6e0003ce4807" >${PORT}
sleep 0.2
echo "rx 00cdf58818ffb9af641f6b0003ce4907" >${PORT}
sleep 0.2
echo "rx 00cdb18918ffbbaf641f690003ce4a07" >${PORT}
sleep 0.2
echo "rx 00cd668a18ffb2af641f680003ce4b07" >${PORT}
sleep 0.2
echo "rx 00cd1b8b18ffaaaf641f670003ce4c07" >${PORT}
sleep 0.2
echo "rx 00cdcd8b18ffaaaf641f650003ce4d07" >${PORT}
sleep 0.2
echo "rx 00cd5f8c18ffb4af641f640003ce4e07" >${PORT}
sleep 0.2
echo "rx 00cdc98c18ffd2af641f620003ce4f07" >${PORT}
sleep 0.2

## state should now be Landed

