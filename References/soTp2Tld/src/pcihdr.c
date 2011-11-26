#include "../include/pcihdr.h"


//función que busca en la tabla los strings asociados a vendor y device code
void
getVenDevStr (unsigned int venCode,unsigned int devCode,char ** venStr,char **devStr)
{
	int i=0;

	while (i<PCI_VENDOR_LEN && venCode>PciVenTable[i].VenId)
		i++;

	if (i>=PCI_VENDOR_LEN || venCode<PciVenTable[i].VenId)
		return;

	*venStr=PciVenTable[i].VenFull;

	i=0;	
	while (i<PCI_DEVICE_LEN && venCode>PciDevTable[i].VenId)
		i++;

	if (i>=PCI_DEVICE_LEN || venCode<PciDevTable[i].VenId)
		return;

	while (i<PCI_DEVICE_LEN && devCode>PciDevTable[i].DevId)
		i++;

	if (i>=PCI_DEVICE_LEN || devCode<PciDevTable[i].DevId)
		return;

	*devStr=PciDevTable[i].ChipDesc;

}
