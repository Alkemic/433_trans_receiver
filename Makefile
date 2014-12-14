all: rpi_receiver

rpi_receiver: RCSwitch.o rpi_receiver.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi -l sqlite3

clean:
	$(RM) *.o rpi_receiver
