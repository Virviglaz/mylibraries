# MyLibraries

A collection of reusable C/C++ libraries and utilities targeting embedded systems, Linux POSIX environments, and bare-metal microcontrollers.

---

## Repository Structure

### `BuildServices/`
Used to help organise a Makefile-based build system. Contains common flags, shared Makefile rules, and embedded-specific build configurations to keep project Makefiles clean and consistent.

### `Common/`
Generic libraries and base interface classes for embedded systems. Includes drivers and abstractions for:
- File I/O (`file_ops`)
- ADC (`ads111x`)
- Sensors (`apds9960`, `BME280`)
- Bit manipulation (`bitops`)
- Argument parsing (`arg_parser`)

### `Linux/`
Linux POSIX-based interface helpers. Provides wrappers and utilities for working with Linux system calls and POSIX APIs in a portable and reusable manner.

### `ARM/`
Low-level hardware drivers for ARM-based SoCs. Currently includes GPIO support for the Allwinner H3 platform.

### `stm32/`, `stm32f103/`, `stm32f10x_cpp/`, `stm32f10x_pr0/`, `stm32f10x_stdperiph_driver/`
Support libraries and peripheral drivers for STM32 (ARM Cortex-M) microcontrollers, including both C and C++ interfaces and the standard peripheral driver library.

### `stm8s/`, `stm8l/`, `stm8s_stdperiph_driver/`
Support libraries and peripheral drivers for STM8 microcontrollers, including the standard peripheral driver library.

---

## Examples

### File I/O (`Common/file_ops`)

**Basic file write and read:**
```cpp
File f("test.txt");

char str[] = "Test string\n";
f.Write(static_cast<void *>(str), sizeof(str));

char rd[sizeof(str)];
f.Seek(0, File::SeekAt::SET);
f.Read(static_cast<void *>(rd), sizeof(rd));

std::cout << "File size: " << f.GetStats().GetSize() << std::endl;
std::cout << "File contents:\n";
std::cout << f;
```

**CSV file parsing:**
```cpp
CSVFile csv("test.csv");
auto csvData = csv.Parse();

for (const auto &line : csvData) {
		for (const auto &value : line) {
			std::visit([](const auto &v) { std::cout << v << " "; }, value.Get());
		}
		std::cout << std::endl;
	}
```

---

## Build

```sh
Create Makefile
Add sources: SRC += example.c or example.cpp
Add include BuildServices/Makefile.embedded
$ make all
```

---

## License

See [BuildServices/LICENSE](BuildServices/LICENSE) for details.