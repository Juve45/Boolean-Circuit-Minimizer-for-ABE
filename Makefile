OBJDIR = obj
SRCDIR = sources
OBJS = $(OBJDIR)/Build.o $(OBJDIR)/Circuit.o $(OBJDIR)/Factorizer.o $(OBJDIR)/InputOutput.o $(OBJDIR)/Logic.o $(OBJDIR)/Random.o $(OBJDIR)/Subcircuit.o $(OBJDIR)/Tree.o 

main: $(OBJS) start/test.cpp
	g++ $(OBJS)  -std=c++17 -O2 -Wall -Wextra start/test.cpp -pthread -o main  -g

run: main
	./main $(FILE)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p ${OBJDIR}
	g++ -c $< -std=c++17 -O2 -Wall -Wextra -o $@ -g

clean:
	rm -rf obj/ main
