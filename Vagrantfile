Vagrant.configure("2") do |config|
  config.vm.guest = :freebsd
  config.vm.box = ENV['FREEBSD_VERSION'] || "freebsd/FreeBSD-15.0-CURRENT"
  config.vm.synced_folder ".", "/vagrant", id: "vagrant-root", disabled: true  
  config.ssh.shell = "sh"
  config.vm.base_mac = "080027D14C66"

  config.vm.provision "shell", inline: <<-SHELL
    pkg update
    pkg upgrade
    pkg install -y python311
    pkg install -y php82-composer
    pkg install -y gitup
    gitup ports
    su vagrant -c "python3.11 -m ensurepip"
    su vagrant -c "python3.11 -m pip install jinja2"
  SHELL
  config.vm.provider "virtualbox" do |vb|
    vb.customize ["modifyvm", :id, "--memory", "1024"]
    vb.customize ["modifyvm", :id, "--cpus", "1"]
    vb.customize ["modifyvm", :id, "--hwvirtex", "on"]
    vb.customize ["modifyvm", :id, "--audio", "none"]
    vb.customize ["modifyvm", :id, "--nictype1", "virtio"]
    vb.customize ["modifyvm", :id, "--nictype2", "virtio"]
  end
end
