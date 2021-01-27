// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2018-2020 The DAPS Project developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/block.h"
#include "consensus/merkle.h"

#include "hash.h"
#include "script/standard.h"
#include "script/sign.h"
#include "tinyformat.h"
#include "utilstrencodings.h"
#include "util.h"

uint256 PoSBlockSummary::GetHash() const {
    return Hash(BEGIN(hash), END(hash),
                BEGIN(nTime), END(nTime),
                BEGIN(height), END(height));
}
/*
int scanhash_scrypt(uint32_t *pdata) {
    uint32_t data[20], hash[8];
    unsigned char scratchbuf = malloc((size_t)N * 128 + 63);
    uint32_t midstate[8];
    uint32_t n = pdata[19] - 1;
    const uint32_t Htarg = ptarget[7];
    int throughput = scrypt_best_throughput();
    int i;

#ifdef HAVE_SHA256_4WAY
    if (sha256_use_4way())
        throughput *= 4;
#endif

    for (i = 0; i < throughput; i++)
        memcpy(data + i * 20, pdata, 80);

    sha256_init(midstate);
    sha256_transform(midstate, data, 0);

#if defined(HAVE_SHA256_4WAY)
        if (throughput == 4)
            scrypt_1024_1_1_256_4way(data, hash, midstate, scratchbuf, N);
        else
#endif
#if defined(HAVE_SCRYPT_3WAY) && defined(HAVE_SHA256_4WAY)
        if (throughput == 12)
            scrypt_1024_1_1_256_12way(data, hash, midstate, scratchbuf, N);
        else
#endif
#if defined(HAVE_SCRYPT_6WAY)
        if (throughput == 24)
            scrypt_1024_1_1_256_24way(data, hash, midstate, scratchbuf, N);
        else
#endif
#if defined(HAVE_SCRYPT_3WAY)
        if (throughput == 3)
            scrypt_1024_1_1_256_3way(data, hash, midstate, scratchbuf, N);
        else
#endif
        scrypt_1024_1_1_256(data, hash, midstate, scratchbuf, N);
    }
}*/

uint256 CBlockHeader::ComputeMinedHash() const
{
    if (IsPoABlockByVersion()) {
        return Hash(BEGIN(nVersion), END(nVersion),
            BEGIN(hashMerkleRoot), END(hashMerkleRoot),
            BEGIN(hashPrevPoABlock), END(hashPrevPoABlock),
            BEGIN(hashPoAMerkleRoot), END(hashPoAMerkleRoot),
            BEGIN(nTime), END(nTime),
            BEGIN(nBits), END(nBits),
            BEGIN(nNonce), END(nNonce));
    }
    return uint256();
}

uint256 CBlockHeader::GetHash() const
{
    if (IsPoABlockByVersion()) {
        //Only hash necessary fields for PoA block header
        //Dont add nAccumulatorCheckpoint to the hash
        return Hash(BEGIN(hashPrevBlock), END(hashPrevBlock),
            BEGIN(minedHash), END(minedHash));
    }
    if(nVersion < 4) {
        return HashQuark(BEGIN(nVersion), END(nNonce));
    }

    return Hash(BEGIN(nVersion), END(nAccumulatorCheckpoint));
}

uint256 CBlock::BuildPoAMerkleTree(bool* fMutated) const
{
    poaMerkleTree.clear();
    poaMerkleTree.reserve(posBlocksAudited.size() * 2 + 16); // Safe upper bound for the number of total nodes.
    for (std::vector<PoSBlockSummary>::const_iterator it(posBlocksAudited.begin()); it != posBlocksAudited.end(); ++it)
        poaMerkleTree.push_back(it->GetHash());
    int j = 0;
    bool mutated = false;
    for (int nSize = posBlocksAudited.size(); nSize > 1; nSize = (nSize + 1) / 2)
    {
        for (int i = 0; i < nSize; i += 2)
        {
            int i2 = std::min(i+1, nSize-1);
            if (i2 == i + 1 && i2 + 1 == nSize && poaMerkleTree[j+i] == poaMerkleTree[j+i2]) {
                // Two identical hashes at the end of the list at a particular level.
                mutated = true;
            }
            poaMerkleTree.push_back(Hash(BEGIN(poaMerkleTree[j+i]),  END(poaMerkleTree[j+i]),
                                       BEGIN(poaMerkleTree[j+i2]), END(poaMerkleTree[j+i2])));
        }
        j += nSize;
    }
    if (fMutated) {
        *fMutated = mutated;
    }
    return (poaMerkleTree.empty() ? uint256() : poaMerkleTree.back());
}

std::vector<uint256> CBlock::GetPoAMerkleBranch(int nIndex) const
{
    if (poaMerkleTree.empty())
        BuildPoAMerkleTree();
    std::vector<uint256> poaMerkleBranch;
    int j = 0;
    for (int nSize = posBlocksAudited.size(); nSize > 1; nSize = (nSize + 1) / 2)
    {
        int i = std::min(nIndex^1, nSize-1);
        poaMerkleBranch.push_back(poaMerkleTree[j+i]);
        nIndex >>= 1;
        j += nSize;
    }
    return poaMerkleBranch;
}

uint256 CBlock::CheckPoAMerkleBranch(uint256 mhash, const std::vector<uint256>& poaMerkleBranch, int nIndex)
{
    if (nIndex == -1)
        return uint256();
    for (std::vector<uint256>::const_iterator it(poaMerkleBranch.begin()); it != poaMerkleBranch.end(); ++it)
    {
        if (nIndex & 1)
            mhash = Hash(BEGIN(*it), END(*it), BEGIN(mhash), END(mhash));
        else
            mhash = Hash(BEGIN(mhash), END(mhash), BEGIN(*it), END(*it));
        nIndex >>= 1;
    }
    return mhash;
}

std::string CBlock::ToString() const
{
    std::stringstream s;
    if (IsProofOfAudit()) {
        s << strprintf("PoABlock(hash=%s, ver=%d, hashPrevBlock=%s, hashPrevPoABlock=%s, hashMerkleRoot=%s, hashPoAMerkleRoot=%s, minedHash=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u, PoSBlocks=%u)\n",
            GetHash().ToString(),
            nVersion,
            hashPrevBlock.ToString(),
            hashPrevPoABlock.ToString(),
            hashMerkleRoot.ToString(),
            hashPoAMerkleRoot.ToString(),
            minedHash.ToString(),
            nTime, nBits, nNonce,
            vtx.size(),
            posBlocksAudited.size());
    } else {
        s << strprintf("CBlock(hash=%s, ver=%d, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
            GetHash().ToString(),
            nVersion,
            hashPrevBlock.ToString(),
            hashMerkleRoot.ToString(),
            nTime, nBits, nNonce,
            vtx.size());
    }
    for (unsigned int i = 0; i < vtx.size(); i++)
    {
        s << "  " << vtx[i].ToString() << "\n";
    }
    return s.str();
}

void CBlock::print() const
{
    LogPrintf("%s", ToString());
}

// ppcoin: sign block
bool CBlock::SignBlock(const CKeyStore& keystore)
{
    std::vector<valtype> vSolutions;
    txnouttype whichType;

    if(!IsProofOfStake())
    {
        for(unsigned int i = 0; i < vtx[0].vout.size(); i++)
        {
            const CTxOut& txout = vtx[0].vout[i];

            if (!Solver(txout.scriptPubKey, whichType, vSolutions))
                continue;

            if (whichType == TX_PUBKEY)
            {
                // Sign
                CKeyID keyID;
                keyID = CKeyID(uint160(vSolutions[0]));

                CKey key;
                if (!keystore.GetKey(keyID, key))
                    return false;

                //vector<unsigned char> vchSig;
                if (!key.Sign(GetHash(), vchBlockSig))
                     return false;

                return true;
            }
        }
    }
    else
    {
        const CTxOut& txout = vtx[1].vout[1];

        if (!Solver(txout.scriptPubKey, whichType, vSolutions))
            return false;

        if (whichType == TX_PUBKEYHASH)
        {

            CKeyID keyID;
            keyID = CKeyID(uint160(vSolutions[0]));

            CKey key;
            if (!keystore.GetKey(keyID, key))
                return false;

            //vector<unsigned char> vchSig;
            if (!key.Sign(GetHash(), vchBlockSig))
                 return false;

            return true;

        }
        else if(whichType == TX_PUBKEY)
        {
            CKeyID keyID;
            keyID = CPubKey(vSolutions[0]).GetID();
            CKey key;
            if (!keystore.GetKey(keyID, key))
                return false;

            //vector<unsigned char> vchSig;
            if (!key.Sign(GetHash(), vchBlockSig))
                 return false;

            return true;
        }
    }

    LogPrintf("Sign failed\n");
    return false;
}

bool CBlock::CheckBlockSignature() const
{
    if (IsProofOfWork())
        return vchBlockSig.empty();

    std::vector<valtype> vSolutions;
    txnouttype whichType;

    const CTxOut& txout = vtx[1].vout[1];

    if (!Solver(txout.scriptPubKey, whichType, vSolutions))
        return false;

    if (whichType == TX_PUBKEY)
    {
        valtype& vchPubKey = vSolutions[0];
        CPubKey pubkey(vchPubKey);
        if (!pubkey.IsValid())
          return false;

        if (vchBlockSig.empty())
            return false;

        return pubkey.Verify(GetHash(), vchBlockSig);
    }
    else if(whichType == TX_PUBKEYHASH)
    {
        valtype& vchPubKey = vSolutions[0];
        CKeyID keyID;
        keyID = CKeyID(uint160(vchPubKey));
        CPubKey pubkey(vchPubKey);

        if (!pubkey.IsValid())
          return false;

        if (vchBlockSig.empty())
            return false;

        return pubkey.Verify(GetHash(), vchBlockSig);

    }

    return false;
}
