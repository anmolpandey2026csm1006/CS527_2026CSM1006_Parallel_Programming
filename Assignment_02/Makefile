TASKS = Task_01 Task_02 Task_03 Task_04 Task_05

all:
	for t in $(TASKS); do $(MAKE) -C $$t || exit 1; done

check:
	for t in $(TASKS); do $(MAKE) -C $$t check || exit 1; done

bench:
	for t in $(TASKS); do $(MAKE) -C $$t bench || exit 1; done

clean:
	for t in $(TASKS); do $(MAKE) -C $$t clean; done

.PHONY: all check bench clean
