#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "comm_typedef.h"

struct queue
{
	uint8_t front;		/* ����ͷ */
	uint8_t rear;		/* ����β */
	uint32_t depth;		/* �����¼���� */
	uint8_t size;		/* �����¼������ݳߴ� */
	int8_t *event;		/* �����¼�����Ϣ */
};

typedef struct queue  queue_t;
typedef struct queue *queueP_t;

/*
 * ������: queue_empty
 * ����: ���������Ƿ�Ϊ��
 * ����:
 *		queue: Ҫ���Ķ���
 * ���:
 *		(int): ������û�����ݷ���1,���򷵻�0
 */
#define queue_empty(queue)	(queue->front == queue->rear)

/*
 * ������: queue_full
 * ����: ���������Ƿ�Ϊ��
 * ����:
 *		queue: Ҫ���Ķ���
 * ���:
 *		(int): ������Ϊ������1,���򷵻�0
 */
#define queue_full(queue)	(((queue->rear + 1) % (queue->depth)) == queue->front)

/*
 * ������: queue_init
 * ����: ���г�ʼ������̬�ռ�
 * ����:
 *		queue: Ҫ��Ҫ��ʼ���Ķ���
 *		size: ��Ա���ݳߴ�
 * 		depth: ���гߴ磬�������
 * ��� ERROR_CODE:
 *		STATUS_NORMAL:	��ʼ���ɹ�
 * 	   	ERR_POINTER_0:	�����¼��洢��������ʧ��
 *						����Ϊ�ն���
 *		ERR_PARAMETER:	�¼���������
 *						ÿ���¼��洢����̫С
 */
ERROR_CODE queue_init(queueP_t queue, uint16_t size, uint32_t depth);

/*
 * ������: queue_deinit
 * ����: ���н��ʼ�����ͷŶ�̬����ռ�
 *       ����������queue_init��ʼ���Ķ���
 * ����:
 *		queue: Ҫ��Ҫ��ʼ���Ķ���
 * ��� ��
 */
void queue_deinit(queueP_t queue);
/*
 * ������: queue_init2
 * ����: ���г�ʼ������̬�ռ�
 * ����:
 *		queue: Ҫ��Ҫ��ʼ���Ķ���
 *		size: ��Ա���ݳߴ�
 * 		depth: ���гߴ磬�������
 *		data: ���ݳ�ָ�룬�Ƕ�̬�ռ�
 * ��� ERROR_CODE:
 *		STATUS_NORMAL:	��ʼ���ɹ�
 * 	   	ERR_POINTER_0:	�����¼��洢��������ʧ��
 *						����Ϊ�ն���
 *		ERR_PARAMETER:	�¼���������
 *						ÿ���¼��洢����̫С
 */
ERROR_CODE queue_init2(queueP_t queue, uint16_t size, uint32_t depth, int8_t *data);

/*
 * ������: queue_push
 * ����: ������ջ,β��
 * ����:
 *		queue: Ҫ��ջ�Ķ���
 *		src: Ҫ��ջ������ָ��
 * ��� ERROR_CODE:
 *		ERR_FULL: ����������ջʧ��
 *		STATUS_NORMAL: ��ջ�ɹ�
 */
ERROR_CODE queue_push(queueP_t queue, void *src);

/*
 * ������: queue_pop
 * ����: ���г�ջ,ͷ��
 * ����:
 *		queue: Ҫ��ջ�Ķ���
 *		desc: �����ջ�����ָ��
 * ��� ERROR_CODE:
 *		ERR_FULL: ���пգ���ջʧ��
 *		STATUS_NORMAL: ��ջ�ɹ�
 */
ERROR_CODE queue_pop(queueP_t queue, void *dest);

#endif /* _QUEUE_H_ */
