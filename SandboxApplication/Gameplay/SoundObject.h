#pragma once

void InitSound(MxObject& object)
{
    object.Name = "Sound Source";
    auto audio = object.AddComponent<AudioSource>();
    auto debug = object.AddComponent<DebugDraw>();
    debug->RenderSoundBounds = true;
    audio->Load(AssetManager::LoadAudio(R"(D:\repos\MxEngineAudio\Audio\res\test.mp3)"));
    audio->Play();
}