import argparse
from ctypes import *
import os
import subprocess

ROOT_DIR = subprocess.check_output('git rev-parse --show-toplevel'.split()).decode('UTF-8')[:-1]
COMMAND_OPTIONS = ['build', 'clean', 'test']

def clean() -> int:
    command_string = 'make clean'
    print('Cleaning build artifacts')
    try:
        print(subprocess.check_output(command_string.split()).decode('UTF-8')[:-1])
        print('Clean completed successfully')
    except Exception:
        print('Failed to clean project!')
        return -1
    return 0

def build(args) -> int:
    command_string = ['make']
    command_string.append('USE_SDL_GRAPHICS=%d' % args.use_sdl_graphics)
    command_string.append('USE_SDL_AUDIO=%d' % args.use_sdl_audio)
    command_string.append('USE_OPENGL_GRAPHICS=%d' % args.use_opengl_graphics)
    command_string.append('USE_OPENAL_AUDIO=%d' % args.use_openal_audio)

    command_string.append('SHOW_TILE_OUTLINES=%d' % args.show_tile_outlines)
    command_string.append('SHOW_WINDOW=%d' % args.show_window)
    command_string.append('TEST=0')

    try:
        print(subprocess.check_output(command_string).decode('UTF-8')[:-1])
        print('Build completed successfully')
    except Exception:
        print('Failed to build project!')
        return -1
    return 0

def test(args) -> int:
    command_string = ['make', 'test']
    command_string.append('USE_SDL_GRAPHICS=%d' % args.use_sdl_graphics)
    command_string.append('USE_SDL_AUDIO=%d' % args.use_sdl_audio)
    command_string.append('USE_OPENGL_GRAPHICS=%d' % args.use_opengl_graphics)
    command_string.append('USE_OPENAL_AUDIO=%d' % args.use_openal_audio)

    command_string.append('SHOW_TILE_OUTLINES=%d' % args.show_tile_outlines)
    command_string.append('SHOW_WINDOW=%d' % args.show_window)
    command_string.append('TEST=1')

    try:
        print(subprocess.check_output(command_string).decode('UTF-8')[:-1])
        print('Build completed successfully')
    except Exception:
        print('Failed to build project!')
        return -1

    run_test_command = [
        'python3',
        os.path.join(ROOT_DIR, 'test', 'emulator_test.py')
    ]
    print('\nTesting CPU...')
    try:
        print(subprocess.check_output(run_test_command).decode('UTF-8')[:-1])
    except Exception: return 1
    return 0

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('command', choices=COMMAND_OPTIONS,
                        help='Makefile command, options are %s' % ', '.join(COMMAND_OPTIONS))
    parser.add_argument('-v', '--verbose', help='increase output verbosity',
                        action='store_true')
    parser.add_argument('-c', '--clean-first', help='increase output verbosity',
                        action='store_true', default=False)
    # Configure drivers
    parser.add_argument('--use-sdl-graphics', help='Use the SDL2 driver to render emulator display',
                        action='store_true', default=False)
    parser.add_argument('--use-sdl-audio', help='Use the SDL2 driver to drive sound logic',
                        action='store_true', default=False)
    parser.add_argument('--use-opengl-graphics', help='Use the openGL driver to render emulator display',
                        action='store_true', default=False)
    parser.add_argument('--use-openal-audio', help='Use the openAL driver to drive sound logic',
                        action='store_true', default=False)
    # Debug features
    parser.add_argument('--show-tile-outlines', help='Show each tile with a red border',
                        action='store_true', default=False)
    parser.add_argument('--show-window', help='Show window',
                        action='store_true', default=False)
    args = parser.parse_args()

    os.chdir(ROOT_DIR)

    # Can't have two audio/graphics drivers
    assert(not (args.use_sdl_graphics and args.use_opengl_graphics))
    assert(not (args.use_sdl_audio and args.use_openal_audio))

    if args.command == 'clean' or args.clean_first:
        clean()
    if args.command == 'build':
        build(args)
    if args.command == 'test':
        test(args)

if __name__ == '__main__':
    main()