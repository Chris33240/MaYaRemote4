#include "io.h"
//#include "setup.h"
//#include "commands_json.h"
//#include "interfaces/hardwarePresenter.h"

/// @brief Combine deux chemins en un seul.
/// You can combine path1, path2 or file when using this function twice :
/// Exemple:
/// pathA = combinePath_HAL("root", "dir1/dir2")     -> pathA = "/root/dir1/dir2"
/// pathB = combinePath_HAL("path1", "filename.ext") -> pathB = "/root/dir1/dir2/filename.ext"
/// @param path1 Premier chemin.
/// @param path2 Deuxième chemin ou fichier.
/// @return Chemin combiné.
String Io::combinePath(String path1, String path2)
{
    path1.trim();
    path2.trim();
    // Ajouter "/" au début de path si non vide et ne commmence pas déjà par "/"
    if (!path1.isEmpty())
    {
        if (!path1.startsWith("/"))
        {
            path1 = "/" + path1;
        }
    }

    if (!path2.isEmpty())
    {
        if (!path1.endsWith("/") && !path2.startsWith("/"))
        {
            path2 = "/" + path2;
        }
        path1 = path1 + path2;
    }

    return path1;
}