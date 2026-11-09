from pathlib import Path
import shutil

def move_src_files():
    src_dir = Path("Src")
    target_dir = Path("Src/cubemx_autogen/Src")

    files_to_move = [
        file
        for file in src_dir.iterdir()
        if file.is_file() and file.name != "main.c"
    ]

    if not files_to_move:
        print("No CubeMX source files found in Src.")
        return

    # Remove the old Src folder and all of its contents
    if target_dir.exists():
        shutil.rmtree(target_dir)

    # Recreate an empty Src folder
    target_dir.mkdir(parents=True, exist_ok=True)

    # Move the files
    for file in files_to_move:
        destination = target_dir / file.name
        shutil.move(str(file), str(destination))

    print("Moved CubeMX source files to Src/cubemx_autogen/Src.")

def move_inc_dir():
    cubemx_inc_dir = Path("Inc")
    target_inc_dir = Path("Src/cubemx_autogen/Inc")

    if not cubemx_inc_dir.exists():
        print("CubeMX inc directory does not exist")
        return

    target_inc_dir.parent.mkdir(parents=True, exist_ok=True)

    if target_inc_dir.exists():
        shutil.rmtree(target_inc_dir)

    shutil.move(str(cubemx_inc_dir), str(target_inc_dir))
    print("CubeMX inc files moved successfully!")

def move_startup_dir():
    cubemx_startup_dir = Path("Startup")
    target_startup_dir = Path("Src/cubemx_autogen/Startup")

    if not cubemx_startup_dir.exists():
        print("CubeMX Startup directory does not exist")
        return

    # Make sure the parent directory exists
    target_startup_dir.parent.mkdir(parents=True, exist_ok=True)

    # Remove the old Startup directory if it exists
    if target_startup_dir.exists():
        shutil.rmtree(target_startup_dir)

    # Move Startup -> src/cubemx_autogen/Startup
    shutil.move(str(cubemx_startup_dir), str(target_startup_dir))

    print("CubeMX Startup files moved successfully!")

def move_drivers_dir():
    cubemx_drivers_dir = Path("Drivers")
    target_drivers_dir = Path("Src/cubemx_autogen/Drivers")

    if not cubemx_drivers_dir.exists():
        print("CubeMX drivers directory does not exist")
        return

    target_drivers_dir.parent.mkdir(parents=True, exist_ok=True)

    if target_drivers_dir.exists():
        shutil.rmtree(target_drivers_dir)

    shutil.move(str(cubemx_drivers_dir), str(target_drivers_dir))
    print("CubeMX drivers files moved successfully!")


def remove_settings_dir():
    folder = Path(".settings")

    if folder.exists():
        shutil.rmtree(folder)
        print("CubeMX .settings directory removed successfully!")


def remove_unnecessary_files():
    for filename in [".cproject", ".mxproject", ".project"]:
        Path(filename).unlink(missing_ok=True)


def move_linker_scripts():
    destination = Path("Src/cubemx_autogen/ldscripts")
    destination.mkdir(parents=True, exist_ok=True)

    for file in Path(".").glob("STM32*_RAM.ld"):
        shutil.move(str(file), str(destination / file.name))

    for file in Path(".").glob("STM32*_FLASH.ld"):
        shutil.move(str(file), str(destination / file.name))

def alphabetize_ioc_file():
    # Find the .ioc file in the project root
    ioc_file = next(Path(".").glob("*.ioc"), None)

    if ioc_file is None:
        print("No .ioc file found")
        return

    lines = ioc_file.read_text(encoding="utf-8").splitlines()

    header = []
    entries = []

    for line in lines:
        stripped = line.strip()

        if not stripped:
            # Preserve blank lines in the header
            if not entries:
                header.append(line)
            continue

        if stripped.startswith("#"):
            header.append(line)
        else:
            entries.append(line)

    # Sort by the key (everything before the first '=')
    entries.sort(key=lambda line: line.split("=", 1)[0].casefold())

    output = header
    if output and output[-1] != "":
        output.append("")

    output.extend(entries)

    ioc_file.write_text("\n".join(output) + "\n", encoding="utf-8")

    print(f"Alphabetized {ioc_file.name}")

def move_middlewares_dir():
    cubemx_middlewares_dir = Path("Middlewares")
    target_middlewares_dir = Path("Src/cubemx_autogen/Middlewares")

    if not cubemx_middlewares_dir.exists():
        print("CubeMX Middlewares directory does not exist")
        return

    # Make sure the parent directory exists
    target_middlewares_dir.parent.mkdir(parents=True, exist_ok=True)

    # Remove the old Middlewares directory if it exists
    if target_middlewares_dir.exists():
        shutil.rmtree(target_middlewares_dir)

    # Move Middlewares -> src/cubemx_autogen/Middlewares
    shutil.move(str(cubemx_middlewares_dir), str(target_middlewares_dir))

    print("CubeMX Middlewares moved successfully!")

def main():
    remove_settings_dir()
    move_drivers_dir()
    move_inc_dir()
    move_middlewares_dir()
    move_src_files()
    move_startup_dir()
    remove_unnecessary_files()
    move_linker_scripts()
    alphabetize_ioc_file()
    print("Format CubeMX Successful!")

main()