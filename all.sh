tests='T*';

prog=$1;

if [ $# -gt 1 ]; then
	shift;
	tests=$*;
fi

for arg in $tests; do
	echo "";
	echo "Running $arg";
	time $prog < "${arg%.*}"/input > "${arg%.*}"/mine.out
	diff "${arg%.*}"/output "${arg%.*}"/mine.out;
done
