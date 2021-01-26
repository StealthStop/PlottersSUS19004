# Variables set by configure 
CXX      = g++
LD       = g++
CXXFLAGS = -g -O2 --std=c++11
LDFLAGS  = 

# Path to source directories
WDIR    = .
ODIR    = obj

# Enable the maximun warning
CXXFLAGS += -Wall -Wextra -Wpedantic

# Flags for generating auto dependancies
CXXDEPFLAGS = -MMD -MP

##########################################################
#Necessary includes and libraries, the order matters here!
##########################################################

# Include ROOT
INCLUDESDIRS  += $(shell root-config --cflags)

# ROOT libraries
LIBS          += $(shell root-config --glibs)

PROGRAMS = plot_1l_LegacyAna

all: mkobj $(PROGRAMS)

mkobj:
	@mkdir -p $(ODIR)

SRC_EXT = cpp c cc C
SRC_DIR = $(WDIR) 
define compile_rule
$$(ODIR)/%.o : $1/%.$2
	$$(CXX) $$(CXXFLAGS) $$(CXXDEPFLAGS)  $$(INCLUDESDIRS) -o $$@ -c $$<
endef    
$(foreach DIR,$(SRC_DIR),$(foreach EXT,$(SRC_EXT),$(eval $(call compile_rule,$(DIR),$(EXT)))))

plot_1l_LegacyAna: $(ODIR)/plot_1l_LegacyAna.o
	$(LD) $^ $(LIBS) -o $@

# Unlink SusyAnaTools soft link and remove obj directory 
clean:
	@rm -rf $(ODIR)/*.o $(ODIR)/*.so $(ODIR)/*.d $(PROGRAMS) core $(ODIR)

-include $(ODIR)/*.d
