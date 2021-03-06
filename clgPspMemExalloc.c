/* clgPspMemExalloc.c */

#include "clgpspmem_internal.h"

static ClgPspMemOptParams st_default_params = { "CLGPSPMEM", CLGPSPMEM_USER, 0, CLGPSPMEM_LO, NULL };

void *clgPspMemExalloc( SceSize size, ClgPspMemOptParams *opt )
{
	SceUID mb_uid;
	void   *mb_head;
	
	/* オプションがNULLであれば、デフォルト値を使用 */
	if( ! opt ) opt = &st_default_params;
	
	/* 取得サイズが0であれば、最小値で確保 */
	if( ! size ) size = 1;
	
	/* パーティションにメモリを確保 */
	mb_uid = sceKernelAllocPartitionMemory( opt->Partition, opt->Name, opt->Type, sizeof( SceUID ) + size, opt->Addr );
	if( ! is_valid_uid( mb_uid ) ) return NULL;
	
	/* メモリアドレスを取得すると同時に、ブロックIDを記録するエリア分ずらす */
	mb_head = (void *)( (uintptr_t)sceKernelGetBlockHeadAddr( mb_uid ) + sizeof( SceUID ) );
	
	/* アライメントを調整 */
	if( opt->Boundary ){
		if( ! CLGPSPMEM_POWER_OF_TWO( opt->Boundary ) ){
			sceKernelFreePartitionMemory( mb_uid );
			return NULL;
		}
		mb_head = (void *)CLGPSPMEM_ALIGN_ADDR( opt->Boundary, mb_head );
	}
	
	/* 最終的に返すメモリアドレスから、ずらしたエリア分戻ってブロックIDを記録 */
	*( (SceUID *)( (uintptr_t)mb_head - sizeof( SceUID ) ) ) = mb_uid;
	
	return mb_head;
}
