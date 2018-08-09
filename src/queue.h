#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "comm_typedef.h"

struct queue
{
	uint8_t front;		/* 队列头 */
	uint8_t rear;		/* 队列尾 */
	uint32_t depth;		/* 队列事件深度 */
	uint8_t size;		/* 单个事件的数据尺寸 */
	int8_t *event;		/* 队列事件，信息 */
};

typedef struct queue  queue_t;
typedef struct queue *queueP_t;

/*
 * 函数名: queue_empty
 * 功能: 检测队列中是否为空
 * 输入:
 *		queue: 要检测的队列
 * 输出:
 *		(int): 队列中没有数据返回1,否则返回0
 */
#define queue_empty(queue)	(queue->front == queue->rear)

/*
 * 函数名: queue_full
 * 功能: 检测队列中是否为满
 * 输入:
 *		queue: 要检测的队列
 * 输出:
 *		(int): 队列中为满返回1,否则返回0
 */
#define queue_full(queue)	(((queue->rear + 1) % (queue->depth)) == queue->front)

/*
 * 函数名: queue_init
 * 功能: 队列初始化，动态空间
 * 输入:
 *		queue: 要需要初始化的队列
 *		size: 成员数据尺寸
 * 		depth: 队列尺寸，队列深度
 * 输出 ERROR_CODE:
 *		STATUS_NORMAL:	初始化成功
 * 	   	ERR_POINTER_0:	队列事件存储区域申请失败
 *						队列为空对象
 *		ERR_PARAMETER:	事件数量不足
 *						每个事件存储区域太小
 */
ERROR_CODE queue_init(queueP_t queue, uint16_t size, uint32_t depth);

/*
 * 函数名: queue_deinit
 * 功能: 队列解初始化，释放动态申请空间
 *       仅适用于由queue_init初始化的队列
 * 输入:
 *		queue: 要需要初始化的队列
 * 输出 无
 */
void queue_deinit(queueP_t queue);
/*
 * 函数名: queue_init2
 * 功能: 队列初始化，静态空间
 * 输入:
 *		queue: 要需要初始化的队列
 *		size: 成员数据尺寸
 * 		depth: 队列尺寸，队列深度
 *		data: 数据池指针，非动态空间
 * 输出 ERROR_CODE:
 *		STATUS_NORMAL:	初始化成功
 * 	   	ERR_POINTER_0:	队列事件存储区域申请失败
 *						队列为空对象
 *		ERR_PARAMETER:	事件数量不足
 *						每个事件存储区域太小
 */
ERROR_CODE queue_init2(queueP_t queue, uint16_t size, uint32_t depth, int8_t *data);

/*
 * 函数名: queue_push
 * 功能: 队列入栈,尾进
 * 输入:
 *		queue: 要入栈的队列
 *		src: 要入栈的数据指针
 * 输出 ERROR_CODE:
 *		ERR_FULL: 队列满，入栈失败
 *		STATUS_NORMAL: 入栈成功
 */
ERROR_CODE queue_push(queueP_t queue, void *src);

/*
 * 函数名: queue_pop
 * 功能: 队列出栈,头出
 * 输入:
 *		queue: 要出栈的队列
 *		desc: 保存出栈数组的指针
 * 输出 ERROR_CODE:
 *		ERR_FULL: 队列空，出栈失败
 *		STATUS_NORMAL: 出栈成功
 */
ERROR_CODE queue_pop(queueP_t queue, void *dest);

#endif /* _QUEUE_H_ */
