#!/bin/sh
echo "#!/bin/sh" > cccc2.sh
echo -n "cccc --lang=c++ --outdir=../../../build/release/dev/synthese3/doc/html/cccc" >> cccc2.sh
chmod +x cccc2.sh
for i in `ls ../src`; do
	echo -n " ../src/$i/*.h ../src/$i/*.cpp ../src/$i/*.hpp" >> cccc2.sh
done
./cccc2.sh
rm -f cccc2.sh
