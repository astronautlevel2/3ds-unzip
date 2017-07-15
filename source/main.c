#include <string.h>
#include <stdio.h>

#include "minizip/unzip.h"

#include <3ds.h>

int main(int argc, char **argv) {

	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);
    bool ran = false;
    char* files[6] = {"bgm.bcstm", "info.smdh", "bgm.ogg", "icon.png", "body_LZ.bin", "Preview.png"};

	// Main loop
	while (aptMainLoop()) {

		gspWaitForVBlank();

		// Your code goes here
        if (!ran)
        {
            printf("Press start to begin...\n");
            while (true)
            {
                hidScanInput();
                u32 kDown = hidKeysDown();
                if (kDown & KEY_START) break;
            }
            FS_Archive ArchiveSD;
            int ret;
            ret = FSUSER_OpenArchive(&ArchiveSD, ARCHIVE_SDMC, fsMakePath(PATH_EMPTY, ""));
            if (R_FAILED(ret)) printf("RIP archive: %i!\n", R_SUMMARY(ret));
            
            Handle dirHandle;
            ret = FSUSER_OpenDirectory(&dirHandle, ArchiveSD, fsMakePath(PATH_ASCII, "/Themes/Relaxing Space"));
            if (R_SUMMARY(ret) == RS_NOTFOUND)
            {
                FSUSER_CreateDirectory(ArchiveSD, fsMakePath(PATH_ASCII, "/Themes/Relaxing Space"), FS_ATTRIBUTE_DIRECTORY);
                ret = FSUSER_OpenDirectory(&dirHandle, ArchiveSD, fsMakePath(PATH_ASCII, "/Themes/Relaxing Space"));
                if (R_SUMMARY(ret) == RS_NOTFOUND) printf("Something weird happened\n");
            }
            char* zip_path = "/Themes/Relaxing Space.zip";
            unzFile zipHandle = unzOpen(zip_path);
            printf("unzFile loaded\n");
            ran = true;
            if (zipHandle == NULL) 
            {
                printf("oops!\n");
            } else {
                for (int i = 0; i < 6; i++)
                {
                    if (unzLocateFile(zipHandle, files[i], 0) == UNZ_OK)
                    {
                        char file_path[128]; //if your file path is more than 127 characters you have bigger issues
                        strcpy(file_path, "/Themes/Relaxing Space/");
                        strcat(file_path, files[i]);
                        unz_file_info *file_info = malloc(sizeof(unz_file_info));
                        unzGetCurrentFileInfo(zipHandle, file_info, NULL, 0, NULL, 0, NULL, 0);
                        char *file;
                        file = malloc(file_info->uncompressed_size);
                        unzOpenCurrentFile(zipHandle);
                        unzReadCurrentFile(zipHandle, file, file_info->uncompressed_size);
                        unzCloseCurrentFile(zipHandle);
                        printf("%X", file[0]);
                        FSUSER_CreateFile(ArchiveSD, fsMakePath(PATH_ASCII, file_path), 0, file_info->uncompressed_size);
                        Handle con_file;
                        FSUSER_OpenFile(&con_file, ArchiveSD, fsMakePath(PATH_ASCII, file_path), FS_OPEN_WRITE, 0);
                        FSFILE_Write(con_file, NULL, 0, file, file_info->uncompressed_size, FS_WRITE_FLUSH);
                        FSFILE_Close(con_file);
                    }
                }
            }
            
            printf("We're done here...");
        }

        hidScanInput();
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	gfxExit();
	return 0;
}
