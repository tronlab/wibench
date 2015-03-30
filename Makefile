include common/makefile.config

KERNEL_LIST := ${shell find $(WiBench_INSTALL_DIR)/src/kernel -type d -print}
APP_LIST := ${shell find $(WiBench_INSTALL_DIR)/src/app -type d -print}

all: kernel app

kernel: 
	cd $(KERNEL_DIR); make;

app:
	cd $(APP_DIR); make;

clean:
	cd $(WiBench_INSTALL_DIR)/build/kernel; rm -rf *;
	cd $(WiBench_INSTALL_DIR)/build/app; rm -rf *;
	cd $(WiBench_INSTALL_DIR)/src/kernel; make clean;
	cd $(WiBench_INSTALL_DIR)/src/app; make clean;
