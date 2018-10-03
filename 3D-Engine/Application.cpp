#include "Application.h"
#include "parson/parson.h"

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	sceneIntro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleImGui(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	
	// Scenes
	AddModule(sceneIntro);
	AddModule(gui);

	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{
	list<Module*>::const_iterator item = list_modules.begin();

	while(item != list_modules.end())
	{
		delete (*item);
		list_modules.erase(item);
		item = list_modules.begin();
	}
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	list<Module*>::const_iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Init();
		item++;
	}

	// After all Init calls we call Start() in all modules
	LOGI("Application Start --------------");
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start();
		item++;
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();

	fpsLog.push_back(1 / dt);

	if(fpsLog.size() > 75)
	{
		fpsLog.erase(fpsLog.begin());
	}

	msLog.push_back(dt * 1000);

	if (msLog.size() > 75)
	{
		msLog.erase(msLog.begin());
	}
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (!renderer3D->vsync && toCap) {
		float toVsync = dt;
	
		if (capFrames > 0)
			toVsync = 1000 / capFrames;
	
		if (dt < toVsync)
			SDL_Delay(toVsync - dt);
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	list<Module*>::const_iterator item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate(dt);
		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update(dt);
		item++;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate(dt);
		item++;
	}

	FinishUpdate();

	item = list_modules.begin();
	//LOAD & SAVE
	if (canLoad)
	{
		JSON_Value *user_data = json_parse_file("user_data.json");
		if (user_data != NULL)
		{
			while (item != list_modules.end() && ret == UPDATE_CONTINUE)
			{
				JSON_Object* dataObj = json_object(user_data);
				ret = (*item)->Load(dataObj);
				item++;
			}
		}
		canLoad = false;
	}

	item = list_modules.begin();
	if (canSave)
	{
		JSON_Value *user_data = json_parse_file("user_data.json");
		if (user_data == NULL)
			user_data = json_value_init_object();
		
		while (item != list_modules.end() && ret == UPDATE_CONTINUE)
		{
			JSON_Object* dataObj = json_object(user_data);
			ret = (*item)->Save(dataObj);
			item++;
		}
		canSave = false;
	}
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	list<Module*>::const_iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->CleanUp();
		item++;
	}
	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}