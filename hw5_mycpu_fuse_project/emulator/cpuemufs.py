import errno
import os
import time
from stat import S_IFDIR, S_IFREG

import fuse

import device


class CpuEmu(fuse.Operations):
    def __init__(self, cores):
        self.device = device.Device(cores)

        self.dirs = []
        self.files = {
            "/ctrl": self.device.ctrl,
        }

        for i in range(cores):
            unit_dir = f"/unit{i}"
            self.dirs.append(unit_dir)
            self.files[f"{unit_dir}/lram"] = self.device.units[i].lram
            self.files[f"{unit_dir}/pram"] = self.device.units[i].pram

        creation_time = time.time()
        common_fields = {
            "st_uid": os.getuid(),
            "st_gid": os.getgid(),
            "st_ctime": creation_time,
            "st_mtime": creation_time,
            "st_atime": creation_time,
        }

        self.dir_stat = {
            "st_mode": S_IFDIR | 0o755,
            "st_nlink": 2,
            **common_fields,
        }

        self.file_stat = {
            "st_mode": S_IFREG | 0o644,
            "st_nlink": 1,
            "st_size": 0,
            **common_fields,
        }

    def getattr(self, path, fh=None):
        if path == "/" or path in self.dirs:
            return self.dir_stat

        if path in self.files:
            size = len(self.files[path])
            return {
                **self.file_stat,
                "st_size": size,
            }

        raise fuse.FuseOSError(errno.ENOENT)

    def readdir(self, path, fh):
        prefix = [".", ".."]

        if path == "/":
            return prefix + [d[1:] for d in self.dirs] + ["ctrl"]

        if path in self.dirs:
            return prefix + ["lram", "pram"]

        raise fuse.FuseOSError(errno.ENOENT)

    def read(self, path, size, offset, fh):
        if path == "/ctrl":
            values = self.files[path].wait()
            payload = "".join(f"{x}\n" for x in values).encode("utf-8")
            return payload[offset:offset + size]

        data = bytes(self.files[path])
        return data[offset:offset + size]

    def write(self, path, data, offset, fh):
        if path == "/ctrl":
            text = data.decode("utf-8").strip()
            if text:
                self.files[path].append(int(text))
            return len(data)

        target = self.files[path]
        end = offset + len(data)

        if end > len(target):
            target.extend(b"\x00" * (end - len(target)))

        target[offset:end] = data
        return len(data)

    def truncate(self, path, length, fh=None):
        if path == "/ctrl":
            return 0

        target = self.files[path]
        current = len(target)

        if length < current:
            del target[length:]
        elif length > current:
            target.extend(b"\x00" * (length - current))

        return 0