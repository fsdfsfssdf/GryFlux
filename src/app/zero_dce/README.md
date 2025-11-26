# Zero-DCE 低光增强模型推理

## 1. 模块简介

本模块基于 GryFlux 流式计算框架实现 Zero-DCE（或类似的低光增强模型）的 RKNN 推理，采用生产者-任务-消费者架构：

- `source/producer`：读取输入目录中的图像并注入管道。
- `tasks/image_preprocess`：自动将输入图像 resize 到 640×480，转换为 RGB 格式。
- `tasks/rk_runner`：加载 RKNN 模型、执行推理、处理量化或浮点输入/输出。
- `tasks/res_sender`：将模型输出转换为 uint8 BGR 图像。
- `sink/write_consumer`：将增强后的图像保存到输出目录。

## 2. 主要特性

- **模型参数**：
  - 输入尺寸：640×480 (RGB)
  - 输出尺寸：640×480 (RGB)
  - 模型类型：图像增强（非超分辨率）
  
- **与 RealESRGAN 的区别**：
  - 尺寸：256×256 → 640×480
  - 命名空间：`RealESRGAN` → `ZeroDCE`
  - 预处理：自动 resize（不再严格要求输入尺寸）
  - 输出：相同尺寸（无上采样）

## 3. 编译步骤

```bash
mkdir -p build && cd build
cmake ..
cmake --build . --target realesrgan_stream -j$(nproc)
```

## 4. 运行方式

```bash
./src/app/zero_dce/realesrgan_stream <模型路径> <图像目录> [输出目录]
```

**参数说明**：
- `<模型路径>`：RKNN 模型文件路径（.rknn），例如 `/data/models/zero_dce_640x480.rknn`
- `<图像目录>`：包含待处理图片的文件夹（支持 .jpg, .jpeg, .png）
- `[输出目录]`（可选）：结果保存位置，默认 `./outputs`

## 5. 注意事项

- 输入图像会自动 resize 到 640×480
- 输出图像与输入图像同名
- 日志默认写入 `./logs/` 目录
- 确保目标设备已安装 RKNN Runtime (`librknnrt.so`)

## 6. 性能优化

- 通过 `StreamingPipeline::enableProfiling(true)` 已启用性能统计
- 可在日志中查看各节点耗时
- 支持 NPU 核心绑定（默认使用 Core1）