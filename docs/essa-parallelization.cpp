R"(	
- ESSA World Update Step:
	- || Object * before_update
		- Update closest approaches
		- Clear most attracting object
	- update_history_and_date
	- Set forces (tick start)
		- || Object * clear_forces
		- || Object * Object * update_forces_against
	- || Object * Leapfrog step 1 & 2
	- Set forces (tick end)
		- || Object * clear_forces
		- || Object * Object * update_forces_against
	- || Object *
		- Leapfrog step 3
		- History rewind
		- Nonphysical update
			- Trails
			- AP/PE
	
Object Update Threads        				World Update Thread
- before_update								
											- update_history_and_date
- clear_forces & update_forces_against
- Leapfrog 1 & 2
- clear_forces & update_forces_against
- Leapfrog 3 & history & nonphysical
)"

class ObjectThread {
	atomic<bool> current_action_done;
	atomic<bool> tick_done;

	void entry();
}

class WorldThread {
	CV request_tick_cv;
	CV global_sync_cv;
	list<ObjectThread> object_threads;

	void tick();
}

void WorldThread::clear_done_state_on_all_object_threads(){
	for(auto& thr: object_threads)
		thr.current_action_done = false;
}

void WorldThread::object_threads_have_work() {
	bool all_done = true;
	for(auto& thr: object_threads)
	{
		if(!thr.tick_done)
			all_done = false;
	}
	return !all_done;
}

void WorldThread::wait_for_all_object_threads(){
	while(true) {
		bool all_done = true;
		for(auto& thr: object_threads)
		{
			if(!thr.current_action_done)
				all_done = false;
		}
		if (all_done)
			return;
	}
}

void WorldThread::tick() {
	update_history_and_date();

	request_tick_cv.notify_all();
	while(object_threads_have_work()) {
		wait_for_all_object_threads();
		global_sync_cv.notify_all();
	}
}

void ObjectThread::entry() {
	while(true) {
		world_thread.request_tick_cv.wait(); // all rendering etc. is here

		for_each_managed_object(before_update); 
		world_thread.global_sync_cv.wait();
		for_each_managed_object(recalculate_forces); 
		world_thread.global_sync_cv.wait();
		for_each_managed_object(leapfrog_step_1_2); 
		world_thread.global_sync_cv.wait();
		for_each_managed_object(recalculate_forces); 
		world_thread.global_sync_cv.wait();
		for_each_managed_object(leapfrog_step_3_nonphysical); 

		tick_done = true;
	}
}
