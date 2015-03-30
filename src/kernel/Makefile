include ../../common/makefile.config

#ALLFILE_LIST := ${shell find $(WiBench_INSTALL_DIR)/src/kernel -type d -print}
#KERNEL_LIST := $(filter-out $(WiBench_INSTALL_DIR)/src/kernel, $(ALLFILE_LIST))
ALLFILE_LIST := ${sort ${dir ${wildcard $(WiBench_INSTALL_DIR)/src/kernel/*/}}}
KERNEL_LIST := $(filter-out $(WiBench_INSTALL_DIR)/src/kernel/, $(ALLFILE_LIST))


all:
	for kernel in $(KERNEL_LIST); do \
	  cd $$kernel; \
	  make; \
	  cd ..; \
	  done;

clean:
	for kernel in $(KERNEL_LIST); do \
	  cd $$kernel; \
	  make clean; \
	  cd ..; \
	  done;

