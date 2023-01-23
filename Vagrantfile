Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/focal64"
  config.vm.box_check_update = false
  config.vm.synced_folder ".", "/vagrant", disabled: true
  config.vm.boot_timeout = 1200

  config.vm.provider "virtualbox" do |vb|
    vb.name = "Master CSI Obfuscation" # friendly name that shows up in Oracle VM VirtualBox Manager
    vb.memory = 4096 # memory in megabytes
    vb.cpus = 3 # cpu cores, can't be more than the host actually has!
  end

  config.vm.provision :shell, path: "docker/install-essentials.sh"
  config.vm.provision :shell, inline: <<-SHELL
    test -f idafree70_linux.run ||  { wget https://out7.hex-rays.com/files/idafree70_linux.run -O idafree70_linux.run ; chmod +x idafree70_linux.run ; }
    ./idafree70_linux.run --mode unattended --installpassword vagrant
  SHELL
  # some basic deps
  config.vm.provision :shell, path: "docker/build-install-llvm.sh"
end
