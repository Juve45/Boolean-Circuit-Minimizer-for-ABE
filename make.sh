echo 'compiling...'
g++ -std=c++17 -Wall -Wextra sources/*.cpp start/cli.cpp -o optimize
echo 'done, you can run it using:'
echo '  ./optimize [hc|ihc|sa|isa|ch|ich] "formula"'
