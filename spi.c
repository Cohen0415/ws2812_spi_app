/*
*
*   file: spi.c
*   updata: 2024-12-05
*
*/

#include "spi.h"

/*******************************
 * @brief : 初始化 SPI 设备
 * @param : spi_dev - SPI 设备路径（例如 "/dev/spidev0.0"）
 * @return: 成功返回文件描述符（fd），失败返回 -1
 * @note  : 初始化 SPI 接口，配置 SPI 参数（模式、位宽、时钟速度）。
 *          需要确保 SPI 设备路径正确且设备已启用。
 *******************************/
int spi_init(const char *spi_dev)
{
    int fd_spidev;
    int ret; 
    SPI_MODE mode;
    char spi_bits;
    SPI_SPEED spi_speed;

    fd_spidev = open(spi_dev, O_RDWR);
	if (fd_spidev < 0) 
    {
		printf("open %s err\n", spi_dev);
		return -1;
	}

    /* mode */
    mode = SPIMODE0;
    ret = ioctl(fd_spidev, SPI_IOC_WR_MODE, &mode);                // mode 0
    if (ret < 0) 
    {
		printf("SPI_IOC_WR_MODE err\n");
		return -1;
	}

    /* bits per word */
    spi_bits = 8;
    ret = ioctl(fd_spidev, SPI_IOC_WR_BITS_PER_WORD, &spi_bits);   // 8bits 
    if (ret < 0) 
    {
		printf("SPI_IOC_WR_BITS_PER_WORD err\n");
		return -1;
	}

    /* speed */
    spi_speed = (uint32_t)S_8M;
    ret = ioctl(fd_spidev, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);    // 1MHz    
    if (ret < 0) 
    {
		printf("SPI_IOC_WR_MAX_SPEED_HZ err\n");
		return -1;
	}

    return fd_spidev;
}

/*******************************
 * @brief : 向 SPI 总线写入数据并读取数据
 * @param : fd_spidev     - SPI 文件描述符（由 spi_init 返回）
 *          send_buf      - 发送数据的缓冲区
 *          send_buf_len  - 发送数据的长度（字节数）
 *          recv_buf      - 接收数据的缓冲区
 *          recv_buf_len  - 接收数据的长度（字节数）
 * @return: 成功返回 0，失败返回 -1
 * @note  : 通过 SPI 总线发送和接收数据。
 *          发送的数据通过 `send_buf`，接收到的数据存放在 `recv_buf` 中。
 *          确保发送和接收缓冲区大小合理。
 *******************************/
int spi_write_then_read(unsigned int fd_spidev, unsigned char *send_buf, unsigned int send_buf_len, unsigned char *recv_buf, unsigned int recv_buf_len)
{
    struct spi_ioc_transfer	xfer[2];
	int status;

    if(send_buf == NULL || recv_buf == NULL)
        return -1;

    if(send_buf_len < 1 || recv_buf_len < 1)
        return -1;

    memset(xfer, 0, sizeof(xfer));

	xfer[0].tx_buf = (unsigned long)send_buf;
	xfer[0].len = send_buf_len;

	xfer[1].rx_buf = (unsigned long)recv_buf;
	xfer[1].len = recv_buf_len;

	status = ioctl(fd_spidev, SPI_IOC_MESSAGE(2), xfer);
	if (status < 0) 
    {
		perror("SPI_IOC_MESSAGE");
		return -1;
	}

    return 0;
}

/*******************************
 * @brief : 向 SPI 总线写入一个字节数据
 * @param : fd_spidev - SPI 文件描述符（由 spi_init 返回）
 *          data      - 待写入的数据字节
 * @return: 成功返回写入的字节数，失败返回 -1
 * @note  : 通过 SPI 总线发送一个字节的数据。
 *******************************/
int spi_write_byte_data(unsigned int fd_spidev, unsigned char data)
{
    unsigned char buff[1] = {data};   
    return write(fd_spidev, &buff, 1);
}

/*******************************
 * @brief : 向 SPI 总线写入多个字节数据
 * @param : fd_spidev     - SPI 文件描述符（由 spi_init 返回）
 *          send_buf      - 待写入的数据缓冲区
 *          send_buf_len  - 待写入的数据长度（字节数）
 * @return: 成功返回 0，失败返回 -1
 * @note  : 通过 SPI 总线发送多个字节的数据。
 *          确保发送缓冲区大小合理。
 *******************************/
int spi_write_nbyte_data(unsigned int fd_spidev, unsigned char *send_buf, unsigned int send_buf_len)
{
    struct spi_ioc_transfer	xfer[2];
    unsigned char recv_buf[send_buf_len];
	int status;

    if(send_buf == NULL || send_buf_len < 1)
        return -1;

    memset(xfer, 0, sizeof(xfer));
    memset(recv_buf, 0, sizeof(send_buf_len));

	xfer[0].tx_buf = (unsigned long)send_buf;
    xfer[0].rx_buf = (unsigned long)recv_buf;
	xfer[0].len = send_buf_len;

	status = ioctl(fd_spidev, SPI_IOC_MESSAGE(1), xfer);
	if (status < 0) 
    {
		perror("SPI_IOC_MESSAGE");
		return -1;
	}

    return 0;
}

/*******************************
 * @brief : 关闭 SPI 设备
 * @param : fd_spidev - SPI 文件描述符（由 spi_init 返回）
 * @return: 无返回值
 * @note  : 关闭 SPI 设备并释放资源。
 *******************************/
void spi_exit(unsigned int fd_spidev)
{
    if(fd_spidev >= 0)
        close(fd_spidev);
}