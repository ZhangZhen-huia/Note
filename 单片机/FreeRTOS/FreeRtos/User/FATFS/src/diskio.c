/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */

/* Definitions of physical drive number for each drive */
//#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */

#include "bsp_spi.h"
#define SD_CARD		0	/* Example: Map ATA harddisk to physical drive 0 */
#define SPI_FLASH		1	/* Example: Map MMC/SD card to physical drive 1 */
/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;
//	int result;

	switch (pdrv) {
	case SD_CARD :
//		result = ATA_disk_status();

		// translate the reslut code here

		return stat;

	case SPI_FLASH :
//		result = MMC_disk_status();
		if(SPI_FLASH_ReadID() == sFLASH_ID)
		{
			stat = 0;
		}
		else
		{
			stat = STA_NOINIT;
		}
		// translate the reslut code here

		return stat;

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
//	int result;

	switch (pdrv) {
	case SD_CARD :
//		result = ATA_disk_initialize();

		// translate the reslut code here

		return stat;

	case SPI_FLASH :
//		result = MMC_disk_initialize();
			SPI_FLASH_Init();//��ʼ��
			SPI_Flash_WAKEUP();//����
			stat = disk_status(pdrv);
		
		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;
//	int result;

	switch (pdrv) {
	case SD_CARD :
		// translate the arguments here
//		result = ATA_disk_read(buff, sector, count);

	
		// translate the reslut code here

		return res;

	case SPI_FLASH :
		// translate the arguments here
	//һ��������4096���ֽڣ�����ʵ�ֵ�spi��flash������һ���ֽ�һ���ֽڵĶ�
	//���Ե�n�������ĵ�ַ����sector*4096��Ҫ��ȡ���ֽ�������Ҫ��ȡ����������4096���ֽ�
	sector+=512;  	
	SPI_FLASH_BufferRead(buff,sector*4096,count*4096);

//		result = MMC_disk_read(buff, sector, count);
		res = RES_OK;
		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;
//	int result;

	switch (pdrv) {
	case SD_CARD :
		// translate the arguments here

//		result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case SPI_FLASH :
		// translate the arguments here
		//һ��������4096���ֽڣ�����ʵ�ֵ�spi��flash������һ���ֽ�һ���ֽڵĶ�
	//���Ե�n�������ĵ�ַ����sector*4096��Ҫд����ֽ�������Ҫ��ȡ����������4096���ֽ�
	//д��֮ǰҪ�Ȳ���
	//ƫ��2M���ֽ�
	sector+=512;
	SPI_FLASH_SectorErase(sector*4096);	
	SPI_FLASH_BufferWrite((u8 *)buff,sector*4096,count*4096);

//		result = MMC_disk_write(buff, sector, count);
		res = RES_OK;
		// translate the reslut code here

		return res;

	}

	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
//	int result;

	switch (pdrv) {
	case SD_CARD :

		// Process of the command for the ATA drive

		return res;

	case SPI_FLASH :
				switch(cmd)
				{
					case CTRL_SYNC:
							
						break;
					//����������С
					case GET_SECTOR_COUNT:
						//flashΪ8M�ֽڣ�һ������4kb�ֽڣ�����һ��8*1024/4 = 2048��
						//����Ϊ���ӵ�flashǰ2M���ֽڵĿռ䱻�������ã������ļ�ϵͳֻ�����ú�6M�Ŀռ�
						//2*1024*1024/4096 = 512��2048-512 = 1536
						*(DWORD*)buff = 1536;
						break;
					
					//ÿ�������Ĵ�С
					case GET_SECTOR_SIZE:
						*(WORD*)buff = 4096;
						break;
					
					//������С������Ĵ�С(flash��С������λ������)
					case GET_BLOCK_SIZE:
						*(DWORD*)buff = 1;
					
				}

			res = RES_OK;
		return res;

	}

	return RES_PARERR;
}

DWORD get_fattime(void)
{
	
	return 0;
}

#endif
