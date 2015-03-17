
CXXFLAGS =	-g -Wall -fmessage-length=0 -fomit-frame-pointer -fstack-protector-all -pipe -std=c++11 

OBJS =		automata.o
TARGET =	demo

#--- primary target
.PHONY : all
all : $(TARGET)

#--- compiler targets
demo : $(OBJS) demo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o : %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<



#--- non-file targets
.PHONY : clean
clean :
	rm *.o $(TARGET)


.PHONY : zip
zip :
	zip -q -r demo.zip *
#--zip all the files 

