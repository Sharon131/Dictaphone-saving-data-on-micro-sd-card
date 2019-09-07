/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/


#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sd_card.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


static volatile DSTATUS Stat = STA_NOINIT; /* Disk status */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
){
	if (pdrv == 0){
		return Stat; /* Supports only single drive */
	}

	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
	int result;

	if (pdrv != 1)
		return STA_NOINIT; /* Supports only single drive */
	if ((Stat & STA_NODISK) != 0)
		return Stat; /* No card in the socket */
	
	//clock init
	
	SPI_SelectDevice();
	
	//cmd0
	
	//cm8
	
	
	
	//init process
	
	
	SPI_DeselectDevice();
	
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	bool WasReadSuccessful = false;

	if (pdrv != 0 || count == 0){
		return RES_PARERR;
	}
	if ((Stat & STA_NOINIT) != 0){
		return RES_NOTRDY;
	}

	if (!(CardType & 4)){
		sector *= 512; /* Convert to byte address if needed */
	}
	
	SPI_SelectDevice();

	if (count == 1){
		WasReadSuccessful = SD_SingleBlockRead(buff, sector);
	} else{
		WasReadSuccessful = SD_MultipleBlocksRead(buff, count, sector);
	}
	
	SPI_DeselectDevice();
	
	//waitfor 1
	
	return (!WasReadSuccessful);
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
	int result;

	if (pdrv != 0 || count == 0)
		return RES_PARERR;
	if ((Stat & STA_NOINIT) != 0)
		return RES_NOTRDY;
	if ((Stat & STA_PROTECT) != 0)
		return RES_WRPRT;

	if (!(CardType & 4))
		sector *= 512; /* Convert to byte address if needed */

	if(count == 1){
		
	} else{
		
	}
	
	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	int result;



	return RES_PARERR;
}
