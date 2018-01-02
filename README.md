# Silly Geo

Silly Geo Game written in C++

TODO:

1) MultyPlayer
2) Strange Bug 

ActiveSound.h
FWaveInstance* FActiveSound::FindWaveInstance( const UPTRINT WaveInstanceHash )
{
	FWaveInstance** WaveInstance = WaveInstances.Find(WaveInstanceHash); // here
	return (WaveInstance ? *WaveInstance : nullptr);
}

#---------------------------------------------------------------------------------------------------------------------------------
