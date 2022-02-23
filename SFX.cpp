#include "SFX.hpp"
#include "easyscript/Namespace.hpp"

#include <EASTL/sort.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/FileSystem.h>



namespace Game {
static eastl::string getGenderStr(GhostIdentity::Gender gender) {
    return gender==GhostIdentity::Gender::female?"female":"male";
}

void getFilesInDir(ResourceCache *resCache, ea::vector<ea::string> &files, const eastl::string& dir) {
    ea::vector<ea::string> fres;
    // Get all files in that dir
    resCache->Scan(fres, dir, "*", SCAN_FILES, true);
    // Sort files
    eastl::sort(fres.begin(), fres.end());
    // Append full resource path to files
    for (const auto& file : fres) {
        if (file.ends_with(".xml")) {
            continue;
        }
        files.push_back(dir+file);
    }
}

Sound *getGhostSFX(Context *ctx, RandomEngine& rng, const eastl::string& kind, GhostIdentity::Gender gender) {
    auto resCache = ctx->GetSubsystem<ResourceCache>();
    ea::vector<ea::string> files;
    // Get all files from viable paths
    getFilesInDir(resCache, files, "SFX/ghost/"+getGenderStr(gender)+'/'+kind+'/');
    getFilesInDir(resCache, files, "SFX/ghost/neutral/"+kind+'/');
    // Return random file
    return resCache->GetResource<Sound>(files[rng.GetUInt(0, files.size())]);
}

Sound *getClickSFX(Context *ctx, RandomEngine& rng, const eastl::string& kind) {
    auto resCache = ctx->GetSubsystem<ResourceCache>();
    ea::vector<ea::string> files;
    // Get all files from viable paths
    getFilesInDir(resCache, files, "SFX/click/"+kind+'/');
    // Return random file
    return resCache->GetResource<Sound>(files[rng.GetUInt(0, files.size())]);
}
}
