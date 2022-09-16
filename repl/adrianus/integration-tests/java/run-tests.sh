echo "building project"

./build_project.sh

python3 run-tests.py

./cleanup.sh

echo "done"
