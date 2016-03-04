INCLUDES= -I/usr/local/include/zdb \
          -I$(top_srcdir)/common \
          -I$(top_srcdir)/network \
          -I$(top_srcdir)/protos \
          -I$(top_srcdir)/virtual_client

LDFLAGS  = -L/usr/local/lib

CXXFLAGS = -std=c++11 -Wall -Wno-reorder -g -fPIC
if IS_BUILD_RELEASE
CXXFLAGS += -DNDEBUG -O2
else
CXXFLAGS += -D_DEBUG -O0 -gdwarf-2
endif

bin_PROGRAMS = virtual_client 

virtual_client_LDADD = $(top_srcdir)/common/libcommon.a \
                      $(top_srcdir)/network/libnetwork.a \
                      -lzdb \
                      -lboost_system \
					  -lboost_thread \
                      -lboost_filesystem \
                      -lboost_random \
                      -lprotobuf \
					  -lcurl

virtual_client_SOURCES = $(top_srcdir)/protos/common.pb.cc \
                        $(top_srcdir)/protos/server.pb.cc \
                        $(top_srcdir)/protos/client.pb.cc \
						main/main.cpp \
						main/virtual_client.cpp \
						http/http_listener.cpp \
						http/http_msg_handler.cpp \
						login/login_connector.cpp

virtual_clientdir=$(prefix)/conf/
