#pragma once

void InitSound(MxObject& object)
{
    object.Name = "Sound Source";
    auto audio = object.AddComponent<AudioSource>();
    auto debug = object.AddComponent<DebugDraw>();
    debug->RenderSoundBounds = true;
    audio->Load(AssetManager::LoadAudio("sounds/never-gonna-give-you-up.mp3"_id));
    audio->Play();
}