#include "CMedialMap.h"

void CMedialMap::fillupoutside()
{
  CMap::fillupoutside();
  for (int i = 0; i < NUMROWS-1; ++i)
  {
    for (int j = 0; j < NUMCOLS-1; ++j)
    {
      if (SignMap[i][j]==0)
      {
        if(medial_axis[i][j]==0)
        {
          SignMap[i][j]=255;
        }
      }
    }
  }
}
