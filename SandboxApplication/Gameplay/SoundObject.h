#pragma once

void InitSound(MxObject& object)
{
    auto audio = object.AddComponent<AudioSource>();
    auto debug = object.AddComponent<DebugDraw>();
    debug->RenderSoundBounds = true;
    audio->Load(AssetManager::LoadAudio("sounds/never-gonna-give-you-up.mp3"_id));
    audio->SetVolume(0.2f);
    audio->Play();
}