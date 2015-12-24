include $(MYROOT)/src/make.dep

all: $(CMP_LIB_DIR)/libcustom.a $(CMP_LIB_DIR)/libpkg.a

OBJS =  pkg_api.o \
        pkg_base.o \
        pkg_tomidbase.o \
        pkg_tomid.o \
        pkg_tomsgbase.o \
				pkg_tomsg.o \
				pkg_tomsg_xml.o \
				pkg_tomid_xml.o \
				pkg_base_cup20.o \
				pkg_tomsg_cup20.o \
				pkg_tomid_cup20.o \
				pkg_tomid_tms.o \
				pkg_tomid_bc.o \
				pkg_tomsg_tms.o \
				pkg_tomid_tms_limit.o \
				pkg_tomsg_tms_limit.o \
				pkg_tomid_transfer.o \
				pkg_tomsg_transfer.o \
				pkg_tomsg_bc.o \
	      pkg_tomsg_allinpay.o \
	      pkg_tomid_allinpay.o \
	      pkg_tomsg_tlv.o \
	      pkg_tomid_tlv.o \
        getAlignPos.o \
        getStructAlignNums.o\
	transcode.o

$(CMP_LIB_DIR)/libcustom.a: $(OBJS)
	$(AR) $@ $(OBJS)

$(CMP_LIB_DIR)/libpkg.a: $(OBJS)
	$(AR) $@ $(OBJS)

.c.o:
	$(CC) $(FLAG) $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -f *.o
